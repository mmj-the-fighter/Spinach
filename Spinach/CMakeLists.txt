cmake_minimum_required(VERSION 3.14)
project(SpinachSolution)

# C/C++ Standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(FIRST_EXAMPLE first_example)
set(SECOND_EXAMPLE second_example)


# Platform-specific flags
if(APPLE)
    add_compile_options(-Wno-deprecated-declarations -O3 -Wall -Wextra)
elseif(UNIX AND NOT APPLE)
    add_compile_options(-O3 -Wall -Wextra -pthread)
endif()

# SDL3 - Try system, else fetch from source
find_package(SDL3 QUIET)
if(NOT SDL3_FOUND)
    message(STATUS "SDL3 not found, downloading...")
    include(FetchContent)
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(SDL3)
    add_compile_definitions(SDL3_BUILT_FROM_SOURCE)
else()
    message(STATUS "Found system SDL3")
endif()

# Spinach
# Collect C++ source files for spinach
file(GLOB_RECURSE SPINACH_CPP_SRC CONFIGURE_DEPENDS
    spinach/common/*.cpp
    spinach/core/*.cpp
)


# Collect C sources for spinach
file(GLOB_RECURSE SPINACH_C_SRC CONFIGURE_DEPENDS
    spinach/external/stb/*.c
)

# Collect headers 
file(GLOB_RECURSE SPINACH_H CONFIGURE_DEPENDS
    spinach/common/*.[h|hpp]
    spinach/core/*.[h|hpp]
    spinach/external/stb/*.h
)


# 1. first_example
# Collect C++ source files
file(GLOB_RECURSE FIRST_EXAMPLE_CPP_SRC CONFIGURE_DEPENDS
    first_example/*.cpp
)

# Collect headers 
file(GLOB_RECURSE FIRST_EXAMPLE_CPP_H CONFIGURE_DEPENDS
	first_example/*.h
)

# Create the executable
add_executable(	${FIRST_EXAMPLE}
	${FIRST_EXAMPLE_CPP_SRC}
	${SPINACH_CPP_SRC}
    ${SPINACH_C_SRC}
    ${FIRST_EXAMPLE_CPP_H} 
	${SPINACH_H}
)

target_include_directories(${FIRST_EXAMPLE} PRIVATE
    spinach/common
    spinach/core
    spinach/external/stb
	first_example
)


target_link_libraries(${FIRST_EXAMPLE} PRIVATE SDL3::SDL3)


# 2. second_example
# Collect C++ source files
file(GLOB_RECURSE SECOND_EXAMPLE_CPP_SRC CONFIGURE_DEPENDS
    second_example/*.cpp
	second_example/imageproc/*.cpp
)

# Collect headers 
file(GLOB_RECURSE SECOND_EXAMPLE_CPP_H CONFIGURE_DEPENDS
	second_example/*.h
	second_example/imageproc/*.h
)

# Create the executable
add_executable(	${SECOND_EXAMPLE}
	${SECOND_EXAMPLE_CPP_SRC}
	${SPINACH_CPP_SRC}
    ${SPINACH_C_SRC}
    ${SECOND_EXAMPLE_CPP_H} 
	${SPINACH_H}
)
target_include_directories(${SECOND_EXAMPLE} PRIVATE
    spinach/common
    spinach/core
    spinach/external/stb
	second_example
	second_example/image_proc
)


target_link_libraries(${SECOND_EXAMPLE} PRIVATE SDL3::SDL3)