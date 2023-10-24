cmake_minimum_required(VERSION 3.10)
project(create-data-mod VERSION 1.0)

if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14") #For OSX
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++") #For Ubuntu
endif (APPLE)

add_subdirectory(genieutils EXCLUDE_FROM_ALL)
include_directories(
        "genieutils/include"
        "."
)
add_executable(
    create-data-mod
    create-data-mod.cpp
    patches/community_games.cpp
    patches/exploding_villagers.cpp
    patches/exploding_kings.cpp
    patches/exploding_relic_monks.cpp
    patches/rewarding_snipes.cpp
    patches/matryoshka.cpp
    patches/flying_dutchman.cpp
    patches/kidnap.cpp
    patches/no_wall.cpp
    patches/nomadking.cpp
    patches/random_costs.cpp
    patches/pocket_horse.cpp
    patches/teamwork.cpp
    patches/duplicate_techs.cpp
    patches/multiplied_civ_bonus.cpp
    patches/shared_civ_bonus.cpp
    ids.h
)
set_property(TARGET create-data-mod PROPERTY CXX_STANDARD 17)
target_link_libraries(create-data-mod genieutils)
