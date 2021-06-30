cmake_minimum_required(VERSION 3.0)
project(simkit_kinect)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

include_directories(include)

set(SOURCES
    src/main.cpp
    src/kinect.cpp
    src/utils.cpp
    src/buttons.cpp
    src/settings.cpp
    src/mainwindow.cpp
    include/mainwindow.h
    src/mainwindow.ui)

find_package(jsoncpp CONFIG REQUIRED)
find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Gui CONFIG REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)   


string(JOIN " " CXX_FLAGS_LIST
    -O2

    # https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Available.md#gcc%E2%80%94clang
    -Wall
    -Wextra
    -Wshadow
    -Wnon-virtual-dtor
    -Wconversion
    -Wunused
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
    -pedantic
    -Werror
    )
set(CMAKE_CXX_FLAGS "${CXX_FLAGS_LIST}")

add_executable(simkit_kinect ${SOURCES})
target_link_libraries(simkit_kinect PRIVATE k4a)
target_link_libraries(simkit_kinect PRIVATE k4arecord)
target_link_libraries(simkit_kinect PRIVATE JetsonGPIO)
target_link_libraries(simkit_kinect PRIVATE pthread)
target_link_libraries(simkit_kinect PRIVATE jsoncpp_object jsoncpp_static)
target_link_libraries(simkit_kinect PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)
target_link_libraries(simkit_kinect PRIVATE X11 Xtst)