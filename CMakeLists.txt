cmake_minimum_required(VERSION 3.30)
project(PyroCore)

option(SDL "Enable SDL" ON)
option(DESKTOP "Desktop Mode or Android" ON)
option(PYRO_DEBUG "Debug mode" ON)

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)

include_directories(
        ${Vulkan_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS}
)

file(GLOB_RECURSE SRC_FILES src/*.cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(PyroCore src/main.cpp ${SRC_FILES})
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

if(SDL)
    add_definitions(-DUSE_SDL)
    find_package(SDL3 REQUIRED)
    target_link_libraries(PyroCore SDL3)
endif()