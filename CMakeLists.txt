cmake_minimum_required(VERSION 3.10)
project(create-data-mod VERSION 1.0)
add_subdirectory(genieutils EXCLUDE_FROM_ALL)
include_directories(
        "genieutils/include"
)
add_executable(create-data-mod create-data-mod.cpp patches.cpp ids.h)
target_link_libraries(create-data-mod genieutils)
