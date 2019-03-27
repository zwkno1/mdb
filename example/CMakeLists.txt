cmake_minimum_required (VERSION 2.8)

project(exampledb)

find_package(Threads)
find_package(Boost REQUIRED COMPONENTS filesystem)

include_directories(../src ./ ${Boost_INCLUDE_DIRS})

FILE(GLOB_RECURSE LibFiles "../include/*.h" "./*.h")
add_custom_target(headers SOURCES ${LibFiles})

add_executable(exampledb
	exampledatabase.cpp
	../src/database.cpp
	)

target_link_libraries(exampledb ${Boost_LIBRARIES} rt ${CMAKE_THREAD_LIBS_INIT})

