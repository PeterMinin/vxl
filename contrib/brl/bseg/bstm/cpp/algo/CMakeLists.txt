INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )

SET(bstm_cpp_algo_sources
    bstm_data_similarity_traits.h
    bstm_ingest_boxm2_scene_function.h bstm_ingest_boxm2_scene_function.txx)

AUX_SOURCE_DIRECTORY(Templates bstm_cpp_algo_sources)

ADD_LIBRARY(bstm_cpp_algo  ${bstm_cpp_algo_sources})
TARGET_LINK_LIBRARIES(bstm_cpp_algo bstm_basic boxm2 boxm2_cpp_algo bstm vcl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )