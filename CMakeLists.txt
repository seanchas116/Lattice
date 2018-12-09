cmake_minimum_required(VERSION 2.8.12)

project(Lattice)

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

set(LIBRARIES Qt5::Widgets)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp")
qt5_add_resources(RESOURCES "src/resources.qrc")

add_executable(Shapecraft ${SOURCES} ${HEADERS} ${RESOURCES})
target_link_libraries(Shapecraft ${LIBRARIES})
