cmake_minimum_required(VERSION 3.10)
project(PSN)

add_subdirectory(examples/cmake)

add_library(psnlib INTERFACE)
target_include_directories(psnlib INTERFACE ./include)
# set_target_properties(psnlib PROPERTIES LINKER_LANGUAGE CXX)
