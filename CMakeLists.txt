cmake_minimum_required(VERSION 3.30)
project(PyroCore)

option(SDL "Enable SDL" ON)
option(DESKTOP "Desktop Mode or Android" ON)
option(PYRO_DEBUG "Debug mode" ON)

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders)
file(GLOB_RECURSE SHADERS ${SHADER_DIR}/*.vert ${SHADER_DIR}/*.frag ${SHADER_DIR}/*.comp ${SHADER_DIR}/*.geom ${SHADER_DIR}/*.tesc ${SHADER_DIR}/*.tese)

find_package(Vulkan REQUIRED)

foreach (SHADER IN LISTS SHADERS)
    get_filename_component(FILENAME ${SHADER} NAME)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/${FILENAME}.spv
            COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} -o ${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/${FILENAME}.spv
            DEPENDS ${SHADER}
            COMMENT "Compiling ${FILENAME}"
    )
    list(APPEND SPV_SHADERS ${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/${FILENAME}.spv)
endforeach ()

add_custom_target(shaders ALL DEPENDS ${SPV_SHADERS})

find_package(glm REQUIRED)

include_directories(
        ${Vulkan_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS}
)

file(GLOB_RECURSE SRC_FILES src/*.cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(PyroCore src/main.cpp ${SRC_FILES} ${SHADERS})
target_link_libraries(PyroCore
        Vulkan::Vulkan
        glm::glm
)
if (PYRO_DEBUG)
    add_definitions(-DPYRO_DEBUG)
endif ()

if (DESKTOP)
    ## Setup for desktop
else ()
    ## Setup Android
endif ()

if (SDL)
    add_definitions(-DUSE_SDL)
    find_package(SDL3 REQUIRED)
    target_link_libraries(PyroCore SDL3)
endif ()

add_dependencies(PyroCore shaders)