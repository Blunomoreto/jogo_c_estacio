cmake_minimum_required(VERSION 3.16)
project(orbit_siege C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
    src/lib/main.c
    src/lib/jogo.c
    src/lib/matematica.c
    src/lib/renderizar.c
    src/lib/inimigo.c
    src/lib/particulas.c
    src/lib/projeteis.c
    src/lib/melhorias.c
    src/lib/cenario.c
    src/lib/interface.c
    src/lib/colisao.c
    src/lib/persistencia.c
    src/lib/audio.c
    src/lib/print.c
    src/lib/imagem.c
    src/lib/pastas.c
)

add_executable(orbit_siege ${SOURCES})

if (WIN32)
    target_link_libraries(orbit_siege opengl32 glu32 freeglut winmm)
else()
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    target_link_libraries(orbit_siege OpenGL::GL GLUT::GLUT m)
endif()
