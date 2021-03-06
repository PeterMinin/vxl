// This is brl/bseg/bvpl/util/bvpl_bundler_features_2d.cxx
#include "bvpl_bundler_features_2d.h"
//
#include <vgl/io/vgl_io_point_3d.h>
#include <vnl/io/vnl_io_vector.h>

#include <vcl_cstdlib.h> // for std::exit()
#include <vcl_iterator.h>

void bvpl_bundler_features_2d::write_feature_txt( vcl_string const& filename ) const
{
    vcl_ofstream of(filename.c_str());

    if (!of)
    {
        vcl_cerr << "----ERROR---- bvpl_bundler_features_2d::write_txt\n"
                 << "\tCOULD NOT OPEN FILE: " << filename
                 << " for writing.\n"
                 << __FILE__ << '\n'
                 << __LINE__ << '\n' << vcl_flush;
        vcl_exit(-1);
    }

    point_view_feature_map_type::const_iterator
        p_itr, p_end = this->pt_view_feature_map.end();

    of << "points_3d = [ ";
    for ( p_itr = this->pt_view_feature_map.begin();
          p_itr != p_end; ++p_itr )
    {
        vcl_map<unsigned, vnl_vector<double> >::const_iterator
            v_itr, v_end = p_itr->second.end();

        for ( v_itr = p_itr->second.begin();
              v_itr != v_end; ++v_itr )
        {
            of << v_itr->second << '\n';
        } //end view iteration
    } //end 3d point iteration

}//end bvpl_bundler_features_2d::write_mfile

void bvpl_bundler_features_2d::write_txt( vcl_string const& filename ) const
{
  vcl_ofstream feature_file(filename.c_str());

  if (!feature_file)
  {
    vcl_cerr << "----ERROR---- bvpl_bundler_features_2d::write_txt\n"
             << "\tCOULD NOT OPEN FILE: " << filename
             << " for writing.\n"
             << __FILE__ << '\n'
             << __LINE__ << '\n' << vcl_flush;
    vcl_exit(-1);
  }

  point_view_feature_map_type::const_iterator
    p_itr, p_end = this->pt_view_feature_map.end();

  for ( p_itr = this->pt_view_feature_map.begin();
        p_itr != p_end; ++p_itr )
  {
    //output the 3d point
    feature_file << p_itr->first << '\n';

    vcl_map<unsigned, vnl_vector<double> >::const_iterator
      v_itr, v_end = p_itr->second.end();

    //output the number of views
    feature_file << p_itr->second.size() << '\n';

    for ( v_itr = p_itr->second.begin();
          v_itr != v_end; ++v_itr )
    {
      //output the view number
      feature_file << v_itr->first << '\n';

      //output the vector
      feature_file << v_itr->second << '\n';
    }//end view iteration
  }//end point iteration

  return;
}//end write_txt

void bvpl_bundler_features_2d::b_write( vsl_b_ostream& os ) const
{
  const short version_no = 1;
  vsl_b_write(os, version_no);

  //write the number of points
  vsl_b_write(os, this->pt_view_feature_map.size());

  point_view_feature_map_type::const_iterator
  pt_itr, pt_end = this->pt_view_feature_map.end();

  for ( pt_itr = this->pt_view_feature_map.begin();
        pt_itr != pt_end; ++pt_itr )
  {
    //write the point
    vsl_b_write(os, pt_itr->first);

    //write the number of views
    vsl_b_write(os, pt_itr->second.size());

    vcl_map<unsigned, vnl_vector<double> >::const_iterator
      v_itr, v_end = pt_itr->second.end();

    for ( v_itr = pt_itr->second.begin();
          v_itr != v_end; ++v_itr )
    {
      //write the view number
      vsl_b_write(os, v_itr->first);

      //write the feature
      vsl_b_write(os, v_itr->second);
    }//end view iteration
  }//end point iteration

  return;
}//end b_write

void bvpl_bundler_features_2d::b_read( vsl_b_istream& is )
{
  if ( !is ) return;

  short v;
  vsl_b_read(is,v);

  switch (v)
  {
   case 1:
    {
      //read the number of points
      vcl_size_t npoints;
      vsl_b_read(is,npoints);

      for ( vcl_size_t i = 0; i < npoints; ++i )
      {
        //read the point
        vgl_point_3d<double> pt;
        vsl_b_read(is,pt);

        //read the number of views
        vcl_size_t nviews;
        vsl_b_read(is,nviews);

        vcl_map<unsigned,vnl_vector<double> > view_feature_map;

        for ( vcl_size_t j = 0; j < nviews; ++j )
        {
          //read the view number
          unsigned view_number;
          vsl_b_read(is,view_number);

          //read the feature
          vnl_vector<double> v;
          vsl_b_read(is,v);

          view_feature_map.insert(vcl_make_pair(view_number,v));
        }//end view iteration

        this->pt_view_feature_map.insert(vcl_make_pair(pt,view_feature_map));
      }//end point iteration
      break;
    }//end case 1
   default:
    {
      vcl_cerr << "----ERROR---- bof_bundler_features_2d::b_read\n"
               << "\tUNKNOWN I/O VERSION\n"
               << __FILE__ << '\n'
               << __LINE__ << '\n'
               << vcl_flush;
      vcl_exit(-1);
    }//end default
  }//end switch

  return;
}//end b_read
