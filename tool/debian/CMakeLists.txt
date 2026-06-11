cmake_minimum_required(VERSION 3.10)
project(fcmd VERSION 1.0.0 LANGUAGES C CXX)

find_package(PkgConfig REQUIRED)

set(CMAKE_BUILD_TYPE Debug) 
# set(CMAKE_BUILD_TYPE Release)

file( GLOB APP_SOURCES *.cpp )
add_executable(fcmd ${APP_SOURCES} )

INCLUDE_DIRECTORIES( "./" )

target_link_libraries(fcmd udev )
