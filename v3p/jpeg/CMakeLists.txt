# v3p/jpeg/CMakeLists.txt

project( jpeg C )

include( ${VXL_CMAKE_DIR}/FindJPEG.cmake )

if(NOT VXL_USING_NATIVE_JPEG)

set( jpeg_sources
  jconfig.h
  jdct.h
  jinclude.h
  jmemsys.h
  jmorecfg.h
  jpegint.h
  jpeglib.h
  jversion.h

  jcapimin.c
  jcapistd.c
  jccoefct.c
  jccolor.c
  jcdctmgr.c
  jchuff.c jchuff.h
  jcinit.c
  jcmainct.c
  jcmarker.c
  jcmaster.c
  jcomapi.c
  jcparam.c
  jcphuff.c
  jcprepct.c
  jcsample.c
  jctrans.c
  jdapimin.c
  jdapistd.c
  jdatadst.c
  jdatasrc.c
  jdcoefct.c
  jdcolor.c
  jddctmgr.c
  jdhuff.c jdhuff.h
  jdinput.c
  jdmainct.c
  jdmarker.c
  jdmaster.c
  jdmerge.c
  jdphuff.c
  jdpostct.c
  jdsample.c
  jdtrans.c
  jerror.c jerror.h
  jfdctflt.c
  jfdctfst.c
  jfdctint.c
  jidctflt.c
  jidctfst.c
  jidctint.c
  jidctred.c
  jquant1.c
  jquant2.c
  jutils.c
  jmemmgr.c
  jmemansi.c
)

vxl_add_library( LIBNAME jpeg LIBSRCS ${jpeg_sources} )

endif()
