cmake_minimum_required(VERSION 3.0.0)

cmake_policy(SET CMP0071 NEW)

project(Lattice)

include(vendor/cmrc/CMakeRC.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

if(MSVC)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -fms-extensions")
    add_definitions(-D_MSC_EXTENSIONS)
endif()

add_definitions(-DGLM_FORCE_SWIZZLE)
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
add_definitions(-D_USE_MATH_DEFINES)

find_package(Qt5Widgets REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIRS})
include_directories("vendor/glm")
include_directories("vendor/json/include")
include_directories("vendor/tinyobjloader")
include_directories(${CMAKE_CURRENT_BINARY_DIR}/_cmrc/include)

set(LIBRARIES Qt5::Widgets Boost::system Boost::filesystem)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp")
list(APPEND SOURCES "vendor/tinyobjloader/tiny_obj_loader.cc")

file(GLOB_RECURSE SHADERS "src/*.vert" "src/*.geom" "src/*.frag")
cmrc_add_resource_library(LatticeResources ${SHADERS})

add_executable(Lattice ${SOURCES} ${SHADERS})
target_link_libraries(Lattice ${LIBRARIES} LatticeResources)
