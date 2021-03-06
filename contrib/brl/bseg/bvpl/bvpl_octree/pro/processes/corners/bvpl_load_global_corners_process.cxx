//:
// \file
// \brief
// \author Isabel Restrepo
// \date 21-Jul-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl_octree/bvpl_global_corners.h>

//: global variables
namespace bvpl_load_global_corners_process_globals
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 1;
}


//: sets input and output types
bool bvpl_load_global_corners_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_load_global_corners_process_globals ;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";

  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "bvpl_global_corners_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//: the process
bool bvpl_load_global_corners_process(bprb_func_process& pro)
{
  using namespace bvpl_load_global_corners_process_globals;

  // get inputs
  vcl_string corners_file = pro.get_input<vcl_string>(0);
  bvpl_global_corners *global_corners = new bvpl_global_corners(corners_file);

  // store output
  pro.set_output_val<bvpl_global_corners_sptr>(0, global_corners);

  return true;
}
