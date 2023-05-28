cmake_minimum_required(VERSION 3.16)
project(test_task)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DUNICODE)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_CRT_NONSTDC_NO_WARNINGS)
add_definitions(-D_CRT_NON_CONFORMING_SWPRINTFS)

#include(cmake/QtLocation.cmake)

include(FetchContent)
FetchContent_Declare(alog
  GIT_REPOSITORY https://github.com/ihor-drachuk/alog.git
  GIT_TAG        dev
)
FetchContent_MakeAvailable(alog)

FetchContent_Declare(utils-cpp
  GIT_REPOSITORY https://github.com/ihor-drachuk/utils-cpp.git
  GIT_TAG        dev
)
FetchContent_MakeAvailable(utils-cpp)

add_subdirectory(test_app)
add_subdirectory(test_lib)

