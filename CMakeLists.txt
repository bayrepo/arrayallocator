cmake_minimum_required(VERSION 2.8)

project(bayrepo)

set(CMAKE_BUILD_TYPE Debug)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes)

add_subdirectory (tests) 
add_subdirectory (src)

#install(TARGETS src/${PROJECT_NAME} DESTINATION lib/${PROJECT_NAME})

#file(GLOB HEADERS includes/*.h)
#install(FILES ${HEADERS} DESTINATION include/${PROJECT_NAME})

