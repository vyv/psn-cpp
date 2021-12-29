![psn logo](https://github.com/vyv-ca/psn-cpp/blob/master/doc/psn-logo.png)

# An Open Protocol for On-Stage, Live 3D Position Data

Initially developed as a means for [**VYV**](https://www.vyv.ca)'s [**Photon**](https://www.vyv.ca/products/photon/) Media Server to internally communicate the position of freely-moving projection surfaces, [**PosiStageNet**](https://www.posistage.net/) became an open standard through a close collaboration between [**VYV**](https://www.vyv.ca) and [**MA Lighting**](https://www.malighting.com/), makers of the world-reknowned [**GrandMA2**](https://www.malighting.com/grandma2/) lighting console.

The result is a combined positioning and lighting system that allows for effects of an unparalleled scale, where large numbers of moving lights can precisely follow multiple performers on stage. The protocol’s applications do not stop at lighting – sound designers can use its data to accurately pan sound effects and music automatically according to the action on stage, and automation operators can obtain another level of feedback on the position of motor-driven stage elements – or even set targets. And that’s just the start; the applications of 3D stage positioning systems are only beginning to be explored.

If you have implemented [**PosiStageNet**](https://www.posistage.net/) in your system and would like to be mentioned on the website, or you have used [**PosiStageNet**](https://www.posistage.net/) for a project and would like to submit material for our Projects Showcase, or for other enquiries, please contact us at **info@posistage.net**

# C++ Implementation

This package contains the documentation and a C++ implementation of the protocol. It also contains three simple examples of how to use the implementation from the server or the client side.

The implementation only uses ".hpp" files. If you want to use this implementation in your project, you need to include the file "psn_lib.hpp" in one of your ".cpp" file.  

The implementation is cross-platform so it should compile under Windows, Linux and Mac OS X. 

For any question about the protocol, please write at **info@posistage.net**

## CMake compilation
You can compile the library and the examples using CMake (for Windows, MacOS and Linux)
- `git submodule update --init --recursive` (fetches the CppSockets library from https://github.com/simondlevy/CppSockets - only required for the examples to work; otherwise bring your own socket library)
- `mkdir build`
- `cd build`
- `cmake ..`
- `cmake --build .`

Now you should be example to run examples as follows:
- `./examples/cmake/send_example` : encode and send PSN messages using UDP ("PSN Server")
- `./examples/cmake/receive_example` : receive PSN messages via UDP and decode ("PSN Client")

## Installing this library using CMake
To install this library in your own CMake-based project, you could use a `CMakeList.txt` similar to the following, assuming you copied or cloned psn-cpp to a directory named `./libs/psn-cpp` relative to the project root:
```
cmake_minimum_required(VERSION 3.19)

project(my_project)

# --- Optional: add CppSockets library if you need it
add_library(cppsockets INTERFACE)
set_property(TARGET cppsockets PROPERTY CXX_STANDARD 11)
target_include_directories(cppsockets INTERFACE ./libs/CppSockets)
# ----

add_executable(my_executable src/my_executable.cpp)
set_property(TARGET my_executable PROPERTY CXX_STANDARD 11)

add_subdirectory(./libs/psn-cpp)

target_include_directories(my_executable PUBLIC ./libs/tether/base_agent/cpp/src)

target_link_libraries(my_executable PUBLIC psnlib cppsockets)
```
