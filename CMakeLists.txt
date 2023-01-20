cmake_minimum_required(VERSION 3.20)
project(thalia)

add_definitions("-std=c++11 -Wall")

set(CMAKE_CXX_STANDARD 11)
set(THALIA_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB THALIA_SOURCES "${THALIA_SOURCE_DIR}/**/*.cpp")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(${THALIA_SOURCE_DIR})
add_executable(thalia ${THALIA_SOURCE_DIR}/main.cpp ${THALIA_SOURCES})
target_link_libraries(thalia ${CONAN_LIBS})

install(TARGETS thalia DESTINATION bin)
