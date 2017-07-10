# swig_wrapper/CMakeLists.txt
file( GLOB_RECURSE SWIG_SRC "*.cxx" "*.i" )
file( GLOB_RECURSE SOFA_INC_FILES "src/*.h" "src/*.hpp" )

add_library(swig_wrapper SHARED ${SWIG_SRC} ${SOFA_INC_FILES} )
target_link_libraries(swig_wrapper gpstk)
target_link_libraries(swig_wrapper POD)


