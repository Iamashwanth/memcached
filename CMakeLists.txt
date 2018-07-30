cmake_minimum_required (VERSION 3.0)

project (objcache)

set(CMAKE_CXX_STANDARD 14)

include_directories("$(CMAKE_CURRENT_SOURCE_DIR)")

file(GLOB SOURCES *.cpp)

add_executable(objcache ${SOURCES})

install(TARGETS objcache DESTINATION bin)
