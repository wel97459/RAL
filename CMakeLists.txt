cmake_minimum_required(VERSION 3.10)
project("Build All")

#Switch build will have a problem with building
#Comment out line 5: include_guard(GLOBAL) in
#/opt/devkitpro/cmake/Platform/NintendoSwitch.cmake

add_subdirectory(fps)
add_subdirectory(cubes)
add_subdirectory(ral_demo)