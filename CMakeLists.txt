cmake_minimum_required(VERSION 3.10)
project(create-data-mod VERSION 1.0)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++")
add_subdirectory(3rd-party/genieutils EXCLUDE_FROM_ALL)
include_directories(
        "3rd-party/genieutils/include"
        "."
)
add_executable(
    create-data-mod
    create-data-mod.cpp
    patches/community_games.cpp
    patches/exploding_villagers.cpp
    patches/flying_dutchman.cpp
    patches/kidnap.cpp
    patches/no_wall.cpp
    patches/random_costs.cpp
    patches/duplicate_techs.cpp
    ids.h
)
set_property(TARGET create-data-mod PROPERTY CXX_STANDARD 17)
target_link_libraries(create-data-mod genieutils)
