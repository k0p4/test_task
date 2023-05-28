cmake_minimum_required(VERSION 3.14)

project(test_app VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

FILE(GLOB_RECURSE SOURCES *.cpp *.h)

find_package(Qt6 COMPONENTS Quick Core Network REQUIRED)

add_executable(test_app ${SOURCES} qml.qrc)

target_link_libraries(test_app PRIVATE Qt6::Quick test_lib)

install(TARGETS test_app
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
