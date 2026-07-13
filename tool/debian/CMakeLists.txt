cmake_minimum_required(VERSION 3.10)
project(fcmd VERSION 1.0.0 LANGUAGES C CXX)

find_package(PkgConfig REQUIRED)

set(CMAKE_BUILD_TYPE Debug) 
# set(CMAKE_BUILD_TYPE Release)

file( GLOB APP_SOURCES source/*.cpp )
add_executable(fcmd ${APP_SOURCES} )

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
    "./source"
    "./")

target_link_libraries(fcmd udev )

target_include_directories(fcmd PRIVATE ${GTK3_INCLUDE_DIRS})
target_compile_options(fcmd PRIVATE -J 12 ${GTK3_CFLAGS_OTHER})

link_directories(${GTK_LIBRARY_DIRS} ${GTKMM_LIBRARY_DIRS})
add_definitions(${GTK_CFLAGS_OTHER})
target_link_libraries(fcmd ${GTK_LIBRARIES} ${GTK3_LIBRARIES} ${GTKMM_LIBRARIES} ${LIBRSVG_LIBRARIES} udev)

install(TARGETS fcmd DESTINATION bin)
