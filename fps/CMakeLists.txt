cmake_minimum_required(VERSION 3.10)
project(fps)
set(EXECUTABLE ${PROJECT_NAME})
set(VERSION 1.0.0)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

if(NINTENDO_SWITCH)
    list(APPEND CMAKE_MODULE_PATH "$ENV{DEVKITPRO}/cmake/")
    include(Switch)
    include(Platform/NintendoSwitch)
endif()


# GLOB all .c files in the src directory
#file(GLOB_RECURSE MyApp_SOURCES src/*.c)
set(MyApp_SOURCES fps.c)
# Add the executable
add_executable(${PROJECT_NAME} ${MyApp_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ..)

# Find required SDL libraries
find_package(SDL2 REQUIRED)
# find_package(SDL2_ttf REQUIRED)
# find_package(SDL2_mixer REQUIRED)

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})

message(STATUS, ${CMAKE_CURRENT_SOURCE_DIR})
# Link the libraries to the executable using the correct target names
target_link_libraries(${PROJECT_NAME} PUBLIC
    SDL2::SDL2
    SDL2::SDL2main
    -lm
    ${NX_LINKER_FLAGS}
    )

if(NINTENDO_SWITCH)
    target_link_options(${PROJECT_NAME} PRIVATE -Wl,-z undefs )
    target_compile_definitions(${PROJECT_NAME} PRIVATE __SWITCH__)
    nx_generate_nacp(OUTPUT "${PROJECT_NAME}.nacp" NAME "${PROJECT_NAME}" AUTHOR "wel97459" VERSION "1.0.0")
    nx_create_nro(${PROJECT_NAME} ROMFS "../romfs" NACP "${PROJECT_NAME}.nacp")
endif()