cmake_minimum_required(VERSION 3.7)

project(gi VERSION 1.0)
set(OpenGL_GL_PREFERENCE GLVND)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)

set(IMGUI_DIR third_party/imgui)

set(imgui_sources
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui ${imgui_sources})
target_include_directories(imgui PRIVATE "${IMGUI_DIR}")
target_include_directories(imgui PRIVATE "${IMGUI_DIR}/backends")

add_library(stb ./third_party/stb/stb_image.c)

file(GLOB_RECURSE sources "${CMAKE_SOURCE_DIR}/lib/*.cc")

add_executable(gi ${sources})

target_compile_options(gi PRIVATE
    -ggdb3
    -Wall -Werror -Wextra
    -Wundef
    -Wcast-align
    -Wsign-conversion
    -Wformat=2
    -Wconversion
    -Wshadow

    -Werror=nonnull
    -Werror=address
    -Werror=init-self
    -Werror=uninitialized
    -Werror=return-type
    -Werror=pointer-arith
    -Werror=implicit-fallthrough
    -Werror=missing-include-dirs
    -Werror=string-compare
    -Werror=switch
    -Werror=write-strings

    -Werror=missing-field-initializers
    -Werror=non-virtual-dtor
    -Werror=pessimizing-move

    # -fsanitize=address)
    )

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(gi PRIVATE
        -Wlogical-op
        -Werror=invalid-memory-model
        -Werror=maybe-uninitialized
        -Werror=return-local-addr)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(gi PRIVATE
        -Werror=dangling
        -Werror=return-stack-address)
endif()

add_subdirectory(third_party/fmt)
find_package(OpenGL REQUIRED)

target_link_libraries(gi stb imgui fmt glfw OpenGL::GL GLEW pthread ${CMAKE_DL_LIBS})

target_include_directories(gi PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(gi PUBLIC "${IMGUI_DIR}")
target_include_directories(gi PUBLIC "${IMGUI_DIR}/backends")

# target_link_options(gi PRIVATE -fsanitize=address)
