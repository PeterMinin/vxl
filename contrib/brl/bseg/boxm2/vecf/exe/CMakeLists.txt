add_executable(boxm2_vecf_fit_orbit_model orbit_fitter.cxx )
target_link_libraries(boxm2_vecf_fit_orbit_model boxm2_vecf ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vpl)

add_executable(boxm2_vecf_orbit_stats orbit_stats.cxx )
target_link_libraries(boxm2_vecf_orbit_stats boxm2_vecf ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vpl)

add_executable(boxm2_vecf_pc_orbit_viewer orbit_pc_viewer.cxx )
target_link_libraries(boxm2_vecf_pc_orbit_viewer boxm2_vecf ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vpl)

add_executable(boxm2_vecf_fit_skull_model skull_fitter.cxx)
target_link_libraries(boxm2_vecf_fit_skull_model boxm2_vecf ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vpl)

