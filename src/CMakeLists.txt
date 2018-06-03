include(GNUInstallDirs)

IF(NOT DEFINED LIB_VERSION_MAJOR)
   set(LIB_VERSION_MAJOR 0)
ENDIF(NOT DEFINED LIB_VERSION_MAJOR)
IF(NOT DEFINED LIB_VERSION_MINOR)
   set(LIB_VERSION_MINOR 1)
ENDIF(NOT DEFINED LIB_VERSION_MINOR)

set(LIB_VERSION_STRING ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR})

file(GLOB LIB_SRC "*.c")
file(GLOB LIB_MAP "../external/map/*.c")
add_definitions(-DMTHREAD)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../includes  ${CMAKE_CURRENT_SOURCE_DIR}/../external/uthash ${CMAKE_CURRENT_SOURCE_DIR}/../external/map)
add_library(bayrepo SHARED ${LIB_SRC} ${LIB_MAP})
add_library(bayrepos STATIC ${LIB_SRC} ${LIB_MAP})

target_link_libraries(bayrepo pthread)
target_link_libraries(bayrepos pthread)

set_target_properties(bayrepo PROPERTIES VERSION ${LIB_VERSION_STRING})
set_target_properties(bayrepos PROPERTIES VERSION ${LIB_VERSION_STRING})

install(TARGETS bayrepo DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME})
install(TARGETS bayrepos DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME})