cmake_minimum_required(VERSION 3.0.0)

cmake_policy(SET CMP0071 NEW)

project(Lattice)

include(vendor/cmrc/CMakeRC.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive-")
else(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
endif()

add_definitions(-DGLM_FORCE_INTRINSICS)
add_definitions(-DGLM_FORCE_SWIZZLE)
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
add_definitions(-D_USE_MATH_DEFINES)

find_package(Qt5Widgets REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIRS})
include_directories("vendor/range-v3/include")
include_directories("vendor/glm")
include_directories("vendor/json/include")
include_directories("vendor/tinyobjloader")
include_directories("vendor/Catch2/single_include")
include_directories("vendor/OpenSubdiv")
include_directories(${CMAKE_CURRENT_BINARY_DIR}/_cmrc/include)

set(LIBRARIES Qt5::Widgets Boost::filesystem)

file(GLOB_RECURSE SOURCES
    "src/*.cpp" "src/*.hpp"
    "vendor/tinyobjloader/tiny_obj_loader.cc"
    "vendor/OpenSubdiv/opensubdiv/far/*.cpp" "vendor/OpenSubdiv/opensubdiv/sdc/*.cpp" "vendor/OpenSubdiv/opensubdiv/vtr/*.cpp")

file(GLOB_RECURSE SHADERS "src/*.vert" "src/*.geom" "src/*.frag")
cmrc_add_resource_library(LatticeResources ${SHADERS})

add_library(LatticeCommon STATIC ${SOURCES} ${HEADERS} ${SHADERS})
target_link_libraries(LatticeCommon ${LIBRARIES} LatticeResources)

if(WIN32)
    set(BUNDLE_TYPE WIN32)
elseif(APPLE)
    set(BUNDLE_TYPE MACOSX_BUNDLE)
endif()

add_executable(Lattice ${BUNDLE_TYPE} "main.cpp")
target_link_libraries(Lattice LatticeCommon)
set_target_properties(Lattice PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)

file(GLOB_RECURSE TEST_SOURCES "test/*.cpp")
add_executable(LatticeTest "main.test.cpp" ${TEST_SOURCES})
target_link_libraries(LatticeTest LatticeCommon)
