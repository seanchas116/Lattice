cmake_minimum_required(VERSION 3.0.0)

project(Lattice)

include(vendor/cmrc/CMakeRC.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

if (NOT MSVC)
    set(CMAKE_CXX_FLAGS "-fms-extensions ${CMAKE_CXX_FLAGS}")
    add_definitions(-D_MSC_EXTENSIONS)
endif()

add_definitions(-DGLM_FORCE_SWIZZLE=1)
add_definitions(-D_USE_MATH_DEFINES)

find_package(Qt5Widgets REQUIRED)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories("vendor/glm")
include_directories("vendor/json/include")
include_directories(${CMAKE_CURRENT_BINARY_DIR}/_cmrc/include)

set(LIBRARIES Qt5::Widgets)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp")

file(GLOB_RECURSE SHADERS "src/*.vert" "src/*.geom" "src/*.frag")
cmrc_add_resource_library(LatticeResources ${SHADERS})

add_executable(Lattice ${SOURCES})
target_link_libraries(Lattice ${LIBRARIES} LatticeResources)
