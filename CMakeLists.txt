cmake_minimum_required(VERSION 3.16)

set(MAIN 1brc)
project(1brc)

# top level properties
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_USE_RELATIVE_PATHS True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wno-unused-parameter -Wno-missing-field-initializers")

# functions
function(import_libraries)
	foreach(name IN LISTS ARGN)
		string(TOUPPER ${name} uppername)
		add_library(lib${name} UNKNOWN IMPORTED)
		find_library(${uppername}_LIB ${name} REQUIRED)
		set_target_properties(lib${name} PROPERTIES IMPORTED_LOCATION ${${uppername}_LIB})
	endforeach()
endfunction()

include_directories(BEFORE ${INCLUDE_DIR})
import_libraries(fmt docopt benchmark benchmark_main pthread)

# main
add_executable(${MAIN} $<TARGET_OBJECTS:objects-0> ${MAIN}.cpp)
target_link_libraries(${MAIN} fmt docopt)

add_executable(${MAIN}-O3 $<TARGET_OBJECTS:objects-3> ${MAIN}.cpp)
target_link_libraries(${MAIN}-O3 fmt docopt)

# bench
add_executable(bench $<TARGET_OBJECTS:objects-3> bench.cpp)
target_link_libraries(bench fmt pthread benchmark benchmark_main)

# source files
set(SOURCE_FILES
	src/diskread.cpp
	src/00-baseline.cpp
	src/01-parse_float.cpp
	src/02-parse_int.cpp
	src/03-parse_temp_branchless.cpp
	src/04-parse_line_branchless.cpp
	src/05-custom_map.cpp
	src/06-mmapio.cpp
)

# object libs
add_library(objects-0 OBJECT ${SOURCE_FILES})
target_compile_options(objects-0 PRIVATE -O0)

add_library(objects-3 OBJECT ${SOURCE_FILES})
target_compile_options(objects-3 PRIVATE -O3)

# entr commands to build/run/test/bench on code change
function(add_watch_target name cmd)
	add_custom_target(watch-${name}
		COMMAND bash -c "while true; do fd -e cpp -e h | entr -ccds \"${cmd}\"; [ $? -eq 2 ] || break; done"
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		VERBATIM
	)
endfunction()

set(makecmd "(cd ${CMAKE_CURRENT_BINARY_DIR} && time make -j`nproc` ${MAIN})")
set(makecmd-all "(cd ${CMAKE_CURRENT_BINARY_DIR} && time make -j`nproc`)")
add_watch_target(rebuild ${makecmd})
add_watch_target(rebuild-all ${makecmd-all})
add_watch_target(rerun "${makecmd} && ${CMAKE_CURRENT_BINARY_DIR}/aoc run 2022")
