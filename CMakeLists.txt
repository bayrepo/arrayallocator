cmake_minimum_required(VERSION 2.6)

project(bayrepo)


IF(NOT DEFINED CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release)
ENDIF(NOT DEFINED CMAKE_BUILD_TYPE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes)

add_subdirectory (tests) 
add_subdirectory (src)

file(GLOB HEADERS includes/bayrepomalloc.h includes/bayrepostrings.h external/uthash/*.h)
install(FILES ${HEADERS} DESTINATION include/${PROJECT_NAME})

