# contrib/brl/bseg/boxm/CMakeLists.txt
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bseg )

set(bstm_basic_sources
    bstm_block_id.h      bstm_block_id.cxx
   )

aux_source_directory(Templates bstm_basic_sources)

vxl_add_library( LIBNAME bstm_basic LIBSRCS  ${bstm_basic_sources})
target_link_libraries(bstm_basic ${VXL_LIB_PREFIX}vcl)

#install the .h .txx and libs

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
