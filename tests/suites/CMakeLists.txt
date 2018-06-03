MACRO(ADD_MODULE file)
    ADD_LIBRARY( ${file} MODULE ${file}.c ../utils.c )
    TARGET_LINK_LIBRARIES( ${file} cunit bayrepo)
    SET_TARGET_PROPERTIES( ${file} PROPERTIES
            PREFIX ""
            LIBRARY_OUTPUT_DIRECTORY "."
    )
ENDMACRO(ADD_MODULE file)

FILE(GLOB C_FILES RELATIVE "${CMAKE_SOURCE_DIR}/tests/suites" "${CMAKE_SOURCE_DIR}/tests/suites/*.c")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/../../includes" 
"${CMAKE_CURRENT_SOURCE_DIR}/../" ${CMAKE_CURRENT_SOURCE_DIR}/../../external/uthash ${CMAKE_CURRENT_SOURCE_DIR}/../../external/map)

FOREACH ( module ${C_FILES} )
    STRING( REGEX REPLACE ".c$" "" module "${module}" )
    MESSAGE(STATUS "Found test suite: ${module}")
    ADD_MODULE(${module})
ENDFOREACH ( module ${MODULES} )


