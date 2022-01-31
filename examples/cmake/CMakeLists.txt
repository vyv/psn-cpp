cmake_minimum_required(VERSION 3.19)
project (psn_cpp_examples)

add_executable(send_example psn_server.cpp)
add_executable(receive_example psn_client.cpp)

set_property(TARGET send_example PROPERTY CXX_STANDARD 17)
set_property(TARGET receive_example PROPERTY CXX_STANDARD 17)

target_include_directories(send_example PUBLIC ../../include)
target_include_directories(send_example PUBLIC ./kissnet)

target_include_directories(receive_example PUBLIC ../../include)
target_include_directories(receive_example PUBLIC ./kissnet)