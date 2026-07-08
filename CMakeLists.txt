cmake_minimum_required(VERSION 3.24)

project(TinyTactics VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/3rd_party" CACHE PATH "FetchContent dependency directory" FORCE)

function(tiny_collect_sources output_variable root_directory)
    # Flow: target source root -> recursive source list -> Engine/Sandbox target.
    # Jump: new folders under this root are picked up after CMake reconfigures.
    file(GLOB_RECURSE collected_sources CONFIGURE_DEPENDS
        "${root_directory}/*.c"
        "${root_directory}/*.cc"
        "${root_directory}/*.cpp"
        "${root_directory}/*.cxx"
        "${root_directory}/*.h"
        "${root_directory}/*.hh"
        "${root_directory}/*.hpp"
        "${root_directory}/*.inl")

    set(${output_variable} ${collected_sources} PARENT_SCOPE)
endfunction()

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE)

FetchContent_Declare(glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v2.0.8
    GIT_SHALLOW TRUE
    SOURCE_SUBDIR cmake)

FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE)

FetchContent_Declare(stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG f58f558c120e9b32c217290b80bad1a0729fbb2c)

FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.9b
    GIT_SHALLOW TRUE)

FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(glfw glad glm stb imgui nlohmann_json)

glad_add_library(glad_gl_core_33 STATIC REPRODUCIBLE API gl:core=3.3)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends)
target_link_libraries(imgui PUBLIC glfw glad_gl_core_33)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

tiny_collect_sources(TINY_TACTICS_ENGINE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/Engine")

add_library(TinyTacticsEngine STATIC ${TINY_TACTICS_ENGINE_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src/Engine" PREFIX "Engine" FILES ${TINY_TACTICS_ENGINE_SOURCES})
target_include_directories(TinyTacticsEngine PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(TinyTacticsEngine PUBLIC
    glfw
    glad_gl_core_33
    glm::glm
    stb
    imgui
    nlohmann_json::nlohmann_json)
target_compile_definitions(TinyTacticsEngine PUBLIC GLFW_INCLUDE_NONE)

tiny_collect_sources(TINY_TACTICS_SANDBOX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/Sandbox")

add_executable(TinyTacticsSandbox ${TINY_TACTICS_SANDBOX_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src/Sandbox" PREFIX "Sandbox" FILES ${TINY_TACTICS_SANDBOX_SOURCES})
target_include_directories(TinyTacticsSandbox PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Sandbox")
target_link_libraries(TinyTacticsSandbox PRIVATE TinyTacticsEngine)
