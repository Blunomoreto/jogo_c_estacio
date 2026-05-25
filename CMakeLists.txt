cmake_minimum_required(VERSION 3.16)
project(orbit_siege C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(src/include)
include_directories(src/third_party/stb)

set(SOURCES
    src/lib/main.c
    src/lib/jogo.c
    src/lib/matematica.c
    src/lib/renderizar.c
    src/lib/desenhar.c
    src/lib/inimigo.c
    src/lib/particulas.c
    src/lib/projeteis.c
    src/lib/melhorias.c
    src/lib/cenario.c
    src/lib/interface.c
    src/lib/colisao.c
    src/lib/persistencia.c
    src/lib/audio.c
    src/lib/imagem.c
    src/lib/imagem_stb.c
    src/lib/pastas.c
    src/lib/vulkan.c
)

add_executable(orbit_siege ${SOURCES})

find_package(Vulkan QUIET)
if (Vulkan_FOUND)
    target_include_directories(orbit_siege PRIVATE ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(orbit_siege ${Vulkan_LIBRARIES})
else()
    set(LOCAL_VULKAN_INCLUDE "${CMAKE_SOURCE_DIR}/src/third_party/vulkan/include")
    if (NOT EXISTS "${LOCAL_VULKAN_INCLUDE}/vulkan/vulkan.h")
        message(FATAL_ERROR "Vulkan headers nao encontrados em ${LOCAL_VULKAN_INCLUDE}. Execute scripts\\setup_libs.ps1 primeiro.")
    endif()
    target_include_directories(orbit_siege PRIVATE "${LOCAL_VULKAN_INCLUDE}")
    if (WIN32)
        target_link_libraries(orbit_siege "C:/Windows/System32/vulkan-1.dll")
    else()
        target_link_libraries(orbit_siege vulkan)
    endif()
endif()

if (WIN32)
    set(FREEGLUT_DIR "${CMAKE_SOURCE_DIR}/src/third_party/freeglut/freeglut")
    target_include_directories(orbit_siege PRIVATE "${FREEGLUT_DIR}/include")
    target_link_directories(orbit_siege PRIVATE "${FREEGLUT_DIR}/lib/x64")
    target_link_libraries(orbit_siege opengl32 glu32 freeglut winmm)
    add_custom_command(TARGET orbit_siege POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${FREEGLUT_DIR}/bin/x64/libfreeglut.dll"
            "$<TARGET_FILE_DIR:orbit_siege>/libfreeglut.dll"
    )
else()
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    target_link_libraries(orbit_siege OpenGL::GL GLUT::GLUT m)
endif()
