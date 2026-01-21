cmake_minimum_required(VERSION 3.10)
project(PhysSim)

set(exec
	exec)
set(src
	src/main.cpp
	src/particle_system.cpp
)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${exec} ${src})

target_include_directories(${exec} PRIVATE
	${CMAKE_SOURCE_DIR}/include
)
target_include_directories(${exec} PRIVATE
	${CMAKE_SOURCE_DIR}/vendor/include
)
target_link_directories(${exec} PRIVATE
	${CMAKE_SOURCE_DIR}/vendor/lib
)
target_link_libraries(${exec} PRIVATE
	raylib
	m
)
