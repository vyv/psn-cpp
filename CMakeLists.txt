cmake_minimum_required(VERSION 3.10)
project(PSN VERSION 2.03 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create header-only library interface
add_library(psn INTERFACE)

# Add include directory
target_include_directories(psn INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Platform-specific settings for network library
if(WIN32)
    target_link_libraries(psn INTERFACE ws2_32)
endif()

# Add examples
add_subdirectory(examples)
