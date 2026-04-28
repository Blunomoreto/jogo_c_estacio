cmake_minimum_required(VERSION 3.16)
project(orbit_siege C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
    src/lib/main.c
    src/lib/game.c
    src/lib/maths.c
    src/lib/render.c
    src/lib/enemy.c
    src/lib/particles.c
    src/lib/projectiles.c
    src/lib/upgrades.c
    src/lib/world.c
    src/lib/ui.c
    src/lib/collision.c
    src/lib/persistence.c
    src/lib/audio.c
    src/lib/screenshot.c
    src/lib/image.c
    src/lib/folders.c
)

add_executable(orbit_siege ${SOURCES})

if (WIN32)
    target_link_libraries(orbit_siege opengl32 glu32 freeglut winmm)
else()
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    target_link_libraries(orbit_siege OpenGL::GL GLUT::GLUT m)
endif()
