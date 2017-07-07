# POD/CMakeLists.txt

file( GLOB_RECURSE MY_SOURCE_FILES "src/*.cpp" "src/*.c" )
file( GLOB_RECURSE MY_INC_FILES "src/*.h" "src/*.hpp" )

set( POD_INC_DIRS "" )

foreach( _headerFile ${MY_INC_FILES} )
    get_filename_component( _dir ${_headerFile} PATH )
    list( APPEND POD_INC_DIRS ${_dir} )
endforeach()

include_directories( ${POD_INC_DIRS} )
include_directories( ../SOFA)
add_executable(POD ${MY_SOURCE_FILES} ${MY_INC_FILES})
target_link_libraries(POD  gpstk)
target_link_libraries(POD  SOFA)