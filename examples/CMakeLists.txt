# PSN Examples CMakeLists.txt

# psn_server executable
add_executable(psn_server
    vs2015/psn_server/psn_server.cpp
)

target_link_libraries(psn_server PRIVATE psn)

# psn_client executable
add_executable(psn_client
    vs2015/psn_client/psn_client.cpp
)

target_link_libraries(psn_client PRIVATE psn)

# psn_glut_client executable (optional - requires GLUT and OpenGL)
find_package(OpenGL)
find_package(GLUT)

if(OpenGL_FOUND AND GLUT_FOUND)
    add_executable(psn_glut_client
        vs2015/psn_glut_client/psn_glut_client.cpp
    )

    target_link_libraries(psn_glut_client PRIVATE psn OpenGL::GL GLUT::GLUT)
    target_include_directories(psn_glut_client PRIVATE ${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})

    # macOS specific: no need to copy DLL files
    # Windows would need post-build copy for glut32.dll or glut64.dll

    message(STATUS "Building psn_glut_client (OpenGL visualizer)")
else()
    if(NOT OpenGL_FOUND)
        message(WARNING "OpenGL not found - skipping psn_glut_client")
    endif()
    if(NOT GLUT_FOUND)
        message(WARNING "GLUT not found - skipping psn_glut_client")
    endif()
endif()
