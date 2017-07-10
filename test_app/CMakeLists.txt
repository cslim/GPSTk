# test_app/CMakeLists.txt

add_executable(test_app test_app.cpp)
target_link_libraries(test_app gpstk)
target_link_libraries(test_app POD)


