file(GLOB LIB_SRC "*.c")
add_definitions(-DMTHREAD)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../includes  ${CMAKE_CURRENT_SOURCE_DIR}/../external/uthash)
add_library(bayrepo SHARED ${LIB_SRC})
add_library(bayrepos STATIC ${LIB_SRC})

target_link_libraries(bayrepo pthread)
target_link_libraries(bayrepos pthread)