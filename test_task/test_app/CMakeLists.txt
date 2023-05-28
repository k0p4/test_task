project(test_app LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

FILE(GLOB_RECURSE SOURCES *.cpp *.h)

find_package(Qt6 COMPONENTS Core Network REQUIRED)

add_executable(test_app
  ${SOURCES}
)

set_property(TARGET test_app PROPERTY CXX_STANDARD 17)
target_link_libraries(test_app PUBLIC Qt6::Core Qt6::Network PRIVATE alog utils-cpp)

# Install
install(TARGETS test_app DESTINATION .)
