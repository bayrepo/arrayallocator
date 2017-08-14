CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
SET(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../includes  ${CMAKE_CURRENT_SOURCE_DIR}/../external/uthash)
add_executable(bayrepo_example1 hash_in_shmem.c)
add_executable(bayrepo_example2 list_in_array.c)

target_link_libraries(bayrepo_example1 pthread bayrepos)
target_link_libraries(bayrepo_example2 pthread bayrepos)

ADD_CUSTOM_TARGET(examples1 COMMAND "bayrepo_example1")
ADD_CUSTOM_TARGET(examples2 COMMAND "bayrepo_example2")