cmake_minimum_required(VERSION 3.16)
project(orbit_siege C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
    src/main.c
    src/game.c
    src/collision.c
    src/persistence.c
    src/audio_winmm.c
    src/screenshot.c
    src/image.c
)

add_executable(orbit_siege ${SOURCES})

if (WIN32)
    target_link_libraries(orbit_siege opengl32 glu32 freeglut winmm)
else()
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    target_link_libraries(orbit_siege OpenGL::GL GLUT::GLUT m)
endif()
