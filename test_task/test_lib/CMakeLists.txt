set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

FILE(GLOB_RECURSE SOURCES *.cpp *.h)
add_library(test_lib STATIC ${SOURCES})

set_property(TARGET test_lib PROPERTY CXX_STANDARD 17)

target_link_libraries(test_lib PUBLIC)
target_include_directories(test_lib PUBLIC .)
