# prip/vmap/CMakeLists.txt

set(vmap_sources

    # vbl extensions

   vbl_controlled_partition.cxx  vbl_controlled_partition.h
   vmap_map_iterator_wrapper.h
   vmap_ptr_sequences.h

    # types

   vmap_types.h
   vmap_kernel.txx               vmap_kernel.h
   vmap_oriented_kernel.txx      vmap_oriented_kernel.h
   vmap_non_oriented_kernel.txx  vmap_non_oriented_kernel.h
   vmap_grid_structure.cxx       vmap_grid_structure.h
   vmap_set_grid_structure.h

    # 2-map

   vmap_2_map.cxx                vmap_2_map.txx            vmap_2_map.h

    # 2-topological map

   vmap_2_tmap.txx               vmap_2_tmap.h
   vmap_2_tmap_elts.cxx          vmap_2_tmap_elts.h
)

vxl_add_library(LIBNAME vmap LIBSRCS ${vmap_sources})
target_link_libraries(vmap ${VXL_LIB_PREFIX}vcl)

#if(BUILD_TESTING)
  add_subdirectory(tests)
#endif()
