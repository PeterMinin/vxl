# vxl/vgl/xio/CMakeLists.txt

set(vgl_xio_sources
  vgl_xio_point_2d.txx vgl_xio_point_2d.h
  vgl_xio_point_3d.txx vgl_xio_point_3d.h
  vgl_xio_box_3d.txx vgl_xio_box_3d.h
  vgl_xio_vector_3d.txx vgl_xio_vector_3d.h
)

aux_source_directory(Templates vgl_xio_sources)

vxl_add_library(LIBNAME ${VXL_LIB_PREFIX}vgl_xio LIBSRCS ${vgl_xio_sources})
if(NOT WIN32)
  set_target_properties( ${VXL_LIB_PREFIX}vgl_xio PROPERTIES COMPILE_FLAGS "-fPIC")
endif()
target_link_libraries( ${VXL_LIB_PREFIX}vgl_xio ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vsl )

if( BUILD_TESTING)
  add_subdirectory(tests)
endif()
