aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_DIR)

file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/)

add_executable(example ${SRC_DIR})
