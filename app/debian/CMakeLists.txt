cmake_minimum_required(VERSION 3.10)
# project(Radio-Sensor-App VERSION 0.1.0 LANGUAGES C CXX)
project(Radio-Sensor-App LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -J 12")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0 gtkmm-3.0 librsvg-2.0)

set(CMAKE_BUILD_TYPE Debug) 
# set(CMAKE_BUILD_TYPE Release)

string(TIMESTAMP "%Y-%m-%d" COMPILE_DATE)
configure_file(
    "${CMAKE_SOURCE_DIR}/include/version.h"
    "${CMAKE_BINARY_DIR}/include/version.h"
)

file( GLOB APP_SOURCES source/*.cpp source/datalogger/*.cpp source/evaluator/*.cpp source/line_recorder/*.cpp)
add_executable(rase ${APP_SOURCES} )

INCLUDE_DIRECTORIES(
    ${GTK_INCLUDE_DIRS}
    ${GTK3_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR}
    "/usr/include" 
    "/usr/include/gdk-pixbuf"
    "/usr/include/gdkmm-3.0"
    "/usr/include/gtk-4.0"
    "/usr/include/librsvg-2.0"
    "./include"
    "./source")

target_include_directories(rase PRIVATE ${GTK3_INCLUDE_DIRS})
target_compile_options(rase PRIVATE -J 12 ${GTK3_CFLAGS_OTHER})

link_directories(${GTK_LIBRARY_DIRS} ${GTKMM_LIBRARY_DIRS})
add_definitions(${GTK_CFLAGS_OTHER})
target_link_libraries(rase ${GTK_LIBRARIES} ${GTK3_LIBRARIES} ${GTKMM_LIBRARIES} ${LIBRSVG_LIBRARIES} udev)

install(TARGETS rase DESTINATION bin)
