cmake_minimum_required(VERSION 3.0)
project(simkit_kinect)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

set(SOURCES
    src/main.cpp)



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
target_link_libraries(simkit_kinect PRIVATE qt5)
#