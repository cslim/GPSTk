# SOFA/CMakeLists.txt

file( GLOB_RECURSE SOFA_SOURCE_FILES "*.cpp" "*.c" )
file( GLOB_RECURSE SOFA_INC_FILES "*.h" "*.hpp" )


set( SOFA_INC_DIRS "" )

foreach( _headerFile ${SOFA_INC_FILES} )
    get_filename_component( _dir ${_headerFile} PATH )
    list( APPEND SOFA_INC_DIRS ${_dir} )
endforeach()

include_directories( ${SOFA_INC_DIRS} )
add_library(SOFA ${SOFA_SOURCE_FILES} ${SOFA_INC_FILES})


