cmake_minimum_required (VERSION 2.8)

project(exampledb)

find_package(Threads)
find_package(Boost REQUIRED COMPONENTS filesystem)

include_directories(../src ./ ${Boost_INCLUDE_DIRS})

add_executable(exampledb
        exampledb.cpp
	../src/database.cpp
        )

add_executable(exampledbreader
        exampledbreader.cpp
        ../src/database.cpp
        )

add_executable(exampledbupdater
        exampledbupdater.cpp
        ../src/database.cpp
        )

target_link_libraries(exampledb ${Boost_LIBRARIES} rt ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(exampledbreader ${Boost_LIBRARIES} rt ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(exampledbupdater ${Boost_LIBRARIES} rt ${CMAKE_THREAD_LIBS_INIT})

