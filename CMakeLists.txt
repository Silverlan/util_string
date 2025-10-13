include(${CMAKE_SOURCE_DIR}/cmake/pr_common.cmake)

set(PROJ_NAME util_string)
pr_add_library(${PROJ_NAME} STATIC)

pr_init_module(${PROJ_NAME})

pr_finalize(${PROJ_NAME})
