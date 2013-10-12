#include "volm_satellite_resources.h"

#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <volm/volm_tile.h>
#include <volm/volm_geo_index2.h>
#include <vgl/vgl_intersection.h>

void add_directories(vcl_string root, vcl_vector<vcl_string>& directories) {
  if (vul_file::is_directory(root))
    directories.push_back(root);
  
  vcl_string glob = root + "/*"; // get everything directory or not
  vul_file_iterator file_it(glob.c_str());
  ++file_it;  // skip .
  ++file_it;  // skip ..
  while (file_it) {
    vcl_string name(file_it());
    if (vul_file::is_directory(name))
      add_directories(name, directories);
    ++file_it;
  }

}

void volm_satellite_resources::add_resource(vcl_string name)
{
  volm_satellite_resource res;
  res.full_path_ = name;
  res.name_ = vul_file::strip_directory(name);
  res.name_ = vul_file::strip_extension(res.name_);
  res.meta_ = new brad_image_metadata(name, "");
  resources_.push_back(res);
}

//: x is lon and y is lat in the bbox, construct bbox with min point to be lower left and max to be upper right and as axis aligned with North-East
volm_satellite_resources::volm_satellite_resources(vgl_box_2d<double>& bbox, double min_size) : bbox_(bbox), min_size_(min_size)
{ 
  this->construct_tree();
}

//: traverse the given path recursively and add each satellite resource
void volm_satellite_resources::add_path(vcl_string path)
{
  vcl_vector<vcl_string> directories;
  add_directories(path, directories);
  if (!directories.size())
    return;
  vcl_cout << "found " << directories.size() << " directories!\n"; 
  
  unsigned start = resources_.size();
  for (unsigned i = 0; i < directories.size(); i++) {
    vcl_string glob = directories[i] + "/*.NTF";
    vul_file_iterator file_it(glob.c_str());
    while (file_it) {
      vcl_string name(file_it());
      //vcl_cout << name << "\n";
      this->add_resource(name);    
      ++file_it;
    }  
    glob = directories[i] + "/*.ntf";
    vul_file_iterator file_it2(glob.c_str());
    while (file_it2) {
      vcl_string name(file_it2());
      //vcl_cout << name << "\n";
      this->add_resource(name);    
      ++file_it2;
    }
  }
  unsigned end = resources_.size();
  this->add_resources(start, end);
}

void volm_satellite_resources::construct_tree()
{
  // construct volm_geo_index2 quad tree with 1.0 degree leaves - satellite images are pretty large, so the leaves need to be large
  root_ = volm_geo_index2::construct_tree<vcl_vector<unsigned> >(bbox_, min_size_);
  vcl_vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root_, leaves);
  vcl_cout << " the number of leaves in the quad tree of satellite resources: " << leaves.size() << '\n';
}

void volm_satellite_resources::add_resources(unsigned start, unsigned end) {
  // insert the ids of the resources
  for (unsigned i = start; i < end; i++) {
    vcl_vector<volm_geo_index2_node_sptr> leaves;
    // CAUTION: x is lat and y is lon in nitf_camera but we want x to be lon and y to be lat, use all the corners of satellite image by inverting x-y to create the bounding box
    vgl_box_2d<double> satellite_footprint;
    satellite_footprint.add(vgl_point_2d<double>(resources_[i].meta_->lower_left_.x(), resources_[i].meta_->lower_left_.y()));
    satellite_footprint.add(vgl_point_2d<double>(resources_[i].meta_->upper_right_.x(), resources_[i].meta_->upper_right_.y()));
    volm_geo_index2::get_leaves(root_, leaves, satellite_footprint);
    for (unsigned j = 0; j < leaves.size(); j++) {
      volm_geo_index2_node<vcl_vector<unsigned> >* leaf_ptr = dynamic_cast<volm_geo_index2_node<vcl_vector<unsigned> >* >(leaves[j].ptr());
      leaf_ptr->contents_.push_back(i);  // push this satellite image to this leave that intersects its footprint
    }
  }
}

//: get a list of ids in the resources_ list that overlap the given rectangular region
void volm_satellite_resources::query(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, vcl_string& band_str, vcl_vector<unsigned>& ids)
{
  vgl_box_2d<double> area(lower_left_lon, upper_right_lon, lower_left_lat, upper_right_lat);
  vcl_vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root_, leaves, area);
  for (unsigned i = 0; i < leaves.size(); i++) {
    volm_geo_index2_node<vcl_vector<unsigned> >* leaf_ptr = dynamic_cast<volm_geo_index2_node<vcl_vector<unsigned> >* >(leaves[i].ptr());
    // check which images overlap with the given bbox
    for (unsigned k = 0; k < leaf_ptr->contents_.size(); k++) {
      unsigned res_id = leaf_ptr->contents_[k];
      // CAUTION: x is lat and y is lon in nitf_camera but we want x to be lon and y to be lat, use all the corners of satellite image by inverting x-y to create the bounding box
      vgl_box_2d<double> sat_box;
      sat_box.add(vgl_point_2d<double>(resources_[res_id].meta_->lower_left_.x(), resources_[res_id].meta_->lower_left_.y()));
      sat_box.add(vgl_point_2d<double>(resources_[res_id].meta_->upper_right_.x(), resources_[res_id].meta_->upper_right_.y()));
      if (resources_[res_id].meta_->band_.compare(band_str) == 0 && vgl_intersection(sat_box, area).area() > 0)
        ids.push_back(res_id);
    }
  }
}
//: query the resources in the given box and output the full paths to the given file
bool volm_satellite_resources::query_print_to_file(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, unsigned& cnt, vcl_string& out_file, vcl_string& band_str)
{
  vcl_vector<unsigned> ids;
  query(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, band_str, ids);
  cnt = ids.size();
  vcl_ofstream ofs(out_file.c_str());
  if (!ofs) {
    vcl_cerr << "In volm_satellite_resources::query_print_to_file() -- cannot open file: " << out_file << vcl_endl;
    return false;
  }
  for (unsigned i = 0; i < ids.size(); i++)
    ofs << resources_[ids[i]].full_path_ << '\n';
  ofs.close();
  return true;
}

//: query the resources in the given box and output the full paths of randomly selected seeds to the given file, 
//  the order of satellites to select seeds from: GeoEye1, WorldView2, WorldView1 and then any others
bool volm_satellite_resources::query_seeds_print_to_file(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, int n_seeds, unsigned& cnt, vcl_string& out_file, vcl_string& band_str)
{
  vcl_vector<unsigned> ids;
  double mid_lon = (lower_left_lon + upper_right_lon) / 2;
  double mid_lat = (lower_left_lat + upper_right_lat) / 2;
  double lower_lon = (lower_left_lon + mid_lon) / 2;
  double lower_lat = (lower_left_lat + mid_lat) / 2;
  double upper_lon = (upper_right_lon + mid_lon) / 2;
  double upper_lat = (upper_right_lat + mid_lat) / 2;
  vcl_cout << "using bbox for scene: " << lower_lon << " " << lower_lat << " " << upper_lon << " " << upper_lat << vcl_endl;
  query(lower_lon, lower_lat, upper_lon, upper_lat, band_str, ids);

  // now select n_seeds among these ones
  vcl_map<vcl_string, vcl_vector<unsigned> > possible_seeds;
  vcl_vector<unsigned> tmp;
  possible_seeds["GeoEye-1"] = tmp;
  possible_seeds["WV01"] = tmp;
  possible_seeds["WV02"] = tmp;
  possible_seeds["other"] = tmp;  
  for (unsigned i = 0; i < ids.size(); i++) {
    if (resources_[ids[i]].meta_->cloud_coverage_percentage_ < 1) {
      vcl_map<vcl_string, vcl_vector<unsigned> >::iterator iter = possible_seeds.find(resources_[ids[i]].meta_->satellite_name_);
      if (iter != possible_seeds.end()) 
        iter->second.push_back(ids[i]);
      else
        possible_seeds["other"].push_back(ids[i]);
    }
  }
#if 0
  vcl_cout << "possible seeds:" << vcl_endl;
  for (vcl_map<vcl_string, vcl_vector<unsigned> >::iterator iter = possible_seeds.begin(); iter != possible_seeds.end(); iter++) {
    vcl_cout << iter->first << "\n";
    for (unsigned k = 0; k < iter->second.size(); k++) {
      vcl_cout << "\t\t" << resources_[iter->second[k]].name_ << "\n";
    }
  }
#endif
  vcl_ofstream ofs(out_file.c_str());
  if (!ofs) {
    vcl_cerr << "In volm_satellite_resources::query_print_to_file() -- cannot open file: " << out_file << vcl_endl;
    return false;
  }
  vcl_vector<brad_image_metadata_sptr> selected_names;
  vcl_vector<vcl_string> names;
  names.push_back("GeoEye-1"); names.push_back("WV02"); names.push_back("WV01"); names.push_back("other");
#if 0
  for (unsigned ii = 0; ii < names.size(); ii++) {
    vcl_string name = names[ii];
    for (unsigned i = 0; i < possible_seeds[name].size(); i++) {
      vcl_cout << resources_[possible_seeds[name][i]].name_;
      vcl_cout << " time " << name << ": " << resources_[possible_seeds[name][i]].meta_->t_.year;
      vcl_cout << " " << resources_[possible_seeds[name][i]].meta_->t_.month;
      vcl_cout << " " << resources_[possible_seeds[name][i]].meta_->t_.day;
      vcl_cout << " " << resources_[possible_seeds[name][i]].meta_->t_.hour;
      vcl_cout << " " << resources_[possible_seeds[name][i]].meta_->t_.min;
      vcl_cout << "\n";
    }
  }
#endif

  cnt = 0;
  bool done = false;
  for (unsigned i = 0; i < possible_seeds["GeoEye-1"].size(); i++) {
    // first check if there is a satellite with the same time
    bool exists = false;
    for (unsigned k = 0; k < selected_names.size(); k++) {
      if (resources_[possible_seeds["GeoEye-1"][i]].meta_->same_time(*selected_names[k].ptr())) {
        exists = true;
        break;
      }
    }
    if (exists) continue;
    selected_names.push_back(resources_[possible_seeds["GeoEye-1"][i]].meta_);

    ofs << resources_[possible_seeds["GeoEye-1"][i]].name_ << '\n';
    //vcl_cout << resources_[possible_seeds["GeoEye-1"][i]].name_ << '\n';
    cnt++;

    if (cnt == n_seeds) {
     done = true;
     break;
    }
  } 
  if (!done) {
    for (unsigned i = 0; i < possible_seeds["WV02"].size(); i++) {
     
     // first check if there is a satellite with the same time
     bool exists = false;
     for (unsigned k = 0; k < selected_names.size(); k++) {
       if (resources_[possible_seeds["WV02"][i]].meta_->same_time(*selected_names[k].ptr())) {
         exists = true;
         break;
       }
     }
     if (exists) continue;
     selected_names.push_back(resources_[possible_seeds["WV02"][i]].meta_);
      
     ofs << resources_[possible_seeds["WV02"][i]].name_ << '\n';
     //vcl_cout << resources_[possible_seeds["WV02"][i]].name_ << '\n';
     cnt++;
     if (cnt == n_seeds) {
       done = true;
       break;
     }
    }
  }
  if (!done) {
    for (unsigned i = 0; i < possible_seeds["WV01"].size(); i++) {
     
     // first check if there is a satellite with the same time
     bool exists = false;
     for (unsigned k = 0; k < selected_names.size(); k++) {
       if (resources_[possible_seeds["WV01"][i]].meta_->same_time(*selected_names[k].ptr())) {
         exists = true;
         break;
       }
     }
     if (exists) continue;
     selected_names.push_back(resources_[possible_seeds["WV01"][i]].meta_);
      
     ofs << resources_[possible_seeds["WV01"][i]].name_ << '\n';
     //vcl_cout << resources_[possible_seeds["WV01"][i]].name_ << '\n';
     cnt++;
     if (cnt == n_seeds) {
       done = true;
       break;
     }
    }
  }
  if (!done) {
    for (unsigned i = 0; i < possible_seeds["other"].size(); i++) {
     
     // first check if there is a satellite with the same time
     bool exists = false;
     for (unsigned k = 0; k < selected_names.size(); k++) {
       if (resources_[possible_seeds["other"][i]].meta_->same_time(*selected_names[k].ptr())) {
         exists = true;
         break;
       }
     }
     if (exists) continue;
     selected_names.push_back(resources_[possible_seeds["other"][i]].meta_);
      
     ofs << resources_[possible_seeds["other"][i]].name_ << '\n';
     //vcl_cout << resources_[possible_seeds["other"][i]].name_ << '\n';
     cnt++;
     if (cnt == n_seeds) {
       done = true;
       break;
     }
    }
  }

  ofs.close();
  return true;
}


//: return the full path of a satellite image given its name, if not found returns empty string
vcl_string volm_satellite_resources::full_path(vcl_string name)
{
  for (unsigned i = 0; i < resources_.size(); i++) {
    if (name.compare(resources_[i].name_) == 0) 
      return resources_[i].full_path_;
  }
  return "";
}

//: binary save self to stream
void volm_satellite_resources::b_write(vsl_b_ostream& os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, min_size_);
  vsl_b_write(os, bbox_.min_x());
  vsl_b_write(os, bbox_.min_y());
  vsl_b_write(os, bbox_.max_x());
  vsl_b_write(os, bbox_.max_y());
  vsl_b_write(os, resources_.size());
  for (unsigned i = 0; i < resources_.size(); i++) {
    resources_[i].b_write(os);
  }
}

//: binary load self from stream
void volm_satellite_resources::b_read(vsl_b_istream& is)
{
  if (!is) return;
  short ver;
  vsl_b_read(is, ver);
  if (ver == 0) {
    vsl_b_read(is, min_size_);
    double min_x, min_y, max_x, max_y;
    vsl_b_read(is, min_x);
    vsl_b_read(is, min_y);
    vsl_b_read(is, max_x);
    vsl_b_read(is, max_y);
    bbox_ = vgl_box_2d<double>(min_x, max_x, min_y, max_y);
    unsigned size;
    vsl_b_read(is, size);
    for (unsigned i = 0; i < size; i++) {
      volm_satellite_resource r;
      r.b_read(is);
      resources_.push_back(r);
    }
    this->construct_tree();
    this->add_resources(0, resources_.size());
  }
  else {
    vcl_cout << "volm_satellite_resources -- unknown binary io version " << ver << '\n';
    return;
  }
}

//: binary save self to stream
void volm_satellite_resource::b_write(vsl_b_ostream& os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, full_path_);
  vsl_b_write(os, name_);
  vsl_b_write(os, full_path_mul_pair_);
  meta_->b_write(os);
}

//: binary load self from stream
void volm_satellite_resource::b_read(vsl_b_istream& is)
{
  if (!is) return;
  short ver;
  vsl_b_read(is, ver);
  if (ver == 0) {
    vsl_b_read(is, full_path_);
    vsl_b_read(is, name_);
    vsl_b_read(is, full_path_mul_pair_);
    brad_image_metadata meta;
    meta.b_read(is);
    meta_ = new brad_image_metadata(meta);
  }
  else {
    vcl_cout << "volm_satellite_resources -- unknown binary io version " << ver << '\n';
    return;
  }
}


//dummy vsl io functions to allow volm_satellite_resources to be inserted into
//brdb as a dbvalue
void vsl_b_write(vsl_b_ostream & os, volm_satellite_resources const &tc)
{ /* do nothing */ }
void vsl_b_read(vsl_b_istream & is, volm_satellite_resources &tc)
{ /* do nothing */ }
void vsl_print_summary(vcl_ostream &os, const volm_satellite_resources &tc)
{ /* do nothing */ }
void vsl_b_read(vsl_b_istream& is, volm_satellite_resources* tc)
{ /* do nothing */ }
void vsl_b_write(vsl_b_ostream& os, const volm_satellite_resources* &tc)
{ /* do nothing */ }
void vsl_print_summary(vcl_ostream& os, const volm_satellite_resources* &tc)
{ /* do nothing */ }
void vsl_b_read(vsl_b_istream& is, volm_satellite_resources_sptr& tc)
{ /* do nothing */ }
void vsl_b_write(vsl_b_ostream& os, const volm_satellite_resources_sptr &tc)
{ /* do nothing */ }
void vsl_print_summary(vcl_ostream& os, const volm_satellite_resources_sptr &tc)
{ /* do nothing */ }
