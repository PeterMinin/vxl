include_directories( ${BRL_INCLUDE_DIR}/bseg )

set(bstm_cpp_algo_sources
    bstm_data_similarity_traits.h
    bstm_ingest_boxm2_scene_function.h bstm_ingest_boxm2_scene_function.txx
    bstm_analyze_coherency_function.h bstm_analyze_coherency_function.cxx
    bstm_refine_blk_in_spacetime_function.h bstm_refine_blk_in_spacetime_function.cxx
    bstm_merge_tt_function.h bstm_merge_tt_function.cxx
    bstm_majority_filter.h bstm_majority_filter.cxx
    bstm_refine_blk_in_space_function.h bstm_refine_blk_in_space_function.txx
    bstm_refine_blk_in_time_function.h bstm_refine_blk_in_time_function.txx
    bstm_copy_data_to_future_function.h bstm_copy_data_to_future_function.txx
    bstm_label_bb_function.h)

aux_source_directory(Templates bstm_cpp_algo_sources)

vxl_add_library( LIBNAME bstm_cpp_algo LIBSRCS  ${bstm_cpp_algo_sources})
target_link_libraries(bstm_cpp_algo bstm_basic boxm2 boxm2_cpp_algo bstm bstm_io ${VXL_LIB_PREFIX}vcl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
