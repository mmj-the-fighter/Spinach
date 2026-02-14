cmake_minimum_required(VERSION 3.14)
project(SpinachSolution)

# C/C++ Standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(EXAMPLE_WITH_RENDER_LOOP example_with_render_loop)
set(EXAMPLE_WITHOUT_RENDER_LOOP example_without_render_loop)
set(SPIROGRAPH_WITH_RMGUI spirograph_with_rmgui)
set(MINE_SWEEPER mine_sweeper)
set(IMAGE_PROCESSING_WITH_RMGUI image_processing_with_rmgui)
set(GAME_OF_LIFE game_of_life)
set(SNAKE_CLONE snake_clone)



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
	spinach/ui/*.cpp
)


# Collect C sources for spinach
file(GLOB_RECURSE SPINACH_C_SRC CONFIGURE_DEPENDS
    spinach/external/stb/*.c
)

# Collect headers 
file(GLOB_RECURSE SPINACH_H CONFIGURE_DEPENDS
    spinach/common/*.[h|hpp]
    spinach/core/*.[h|hpp]
	spinach/ui/*.[h|hpp]
    spinach/external/stb/*.h
)

# Collect headers for nanosvg
file(GLOB_RECURSE NANOSVG_H CONFIGURE_DEPENDS
	spinach/external/nanosvg/*.h
)

# 1. example_with_render_loop
# Collect C++ source files
file(GLOB_RECURSE EXAMPLE_WITH_RENDER_LOOP_CPP_SRC CONFIGURE_DEPENDS
    examples/example_with_render_loop/*.cpp
)

# Collect headers 
file(GLOB_RECURSE EXAMPLE_WITH_RENDER_LOOP_CPP_H CONFIGURE_DEPENDS
	examples/example_with_render_loop/*.h
)

# Create the executable
add_executable(	${EXAMPLE_WITH_RENDER_LOOP}
	${EXAMPLE_WITH_RENDER_LOOP_CPP_SRC}
	${SPINACH_CPP_SRC}
    ${SPINACH_C_SRC}
    ${EXAMPLE_WITH_RENDER_LOOP_CPP_H} 
	${SPINACH_H}
	${NANOSVG_H}
)

target_include_directories(${EXAMPLE_WITH_RENDER_LOOP} PRIVATE
    spinach/common
    spinach/core
    spinach/ui
    spinach/external/stb
	spinach/external/nanosvg
	examples/example_with_render_loop
)


target_link_libraries(${EXAMPLE_WITH_RENDER_LOOP} PRIVATE SDL3::SDL3)


# 2. example_without_render_loop
# Collect C++ source files
file(GLOB_RECURSE EXAMPLE_WITHOUT_RENDER_LOOP_CPP_SRC CONFIGURE_DEPENDS
    examples/example_without_render_loop/*.cpp
	examples/example_without_render_loop/imageproc/*.cpp
)

# Collect headers 
file(GLOB_RECURSE EXAMPLE_WITHOUT_RENDER_LOOP_CPP_H CONFIGURE_DEPENDS
	examples/example_without_render_loop/*.h
	examples/example_without_render_loop/imageproc/*.h
)

# Create the executable
add_executable(	${EXAMPLE_WITHOUT_RENDER_LOOP}
	${EXAMPLE_WITHOUT_RENDER_LOOP_CPP_SRC}
	${SPINACH_CPP_SRC}
    ${SPINACH_C_SRC}
    ${EXAMPLE_WITHOUT_RENDER_LOOP_CPP_H} 
	${SPINACH_H}
	${NANOSVG_H}
)
target_include_directories(${EXAMPLE_WITHOUT_RENDER_LOOP} PRIVATE
    spinach/common
    spinach/core
    spinach/ui
    spinach/external/stb
	spinach/external/nanosvg
	examples/example_without_render_loop
	examples/example_without_render_loop/image_proc
)


target_link_libraries(${EXAMPLE_WITHOUT_RENDER_LOOP} PRIVATE SDL3::SDL3)


# 3. spirograph_with_rmgui
# Collect C++ source files
file(GLOB_RECURSE SPIROGRAPH_WITH_RMGUI_CPP_SRC CONFIGURE_DEPENDS
    examples/spirograph_with_rmgui/*.cpp
)

# Collect headers 
file(GLOB_RECURSE SPIROGRAPH_WITH_RMGUI_CPP_H CONFIGURE_DEPENDS
	examples/spirograph_with_rmgui/*.h
)

# Create the executable
add_executable(	${SPIROGRAPH_WITH_RMGUI}
	${SPIROGRAPH_WITH_RMGUI_CPP_SRC}
	${SPINACH_CPP_SRC}
    ${SPINACH_C_SRC}
    ${SPIROGRAPH_WITH_RMGUI_CPP_H} 
	${SPINACH_H}
	${NANOSVG_H}
)
target_include_directories(${SPIROGRAPH_WITH_RMGUI} PRIVATE
    spinach/common
    spinach/core
    spinach/ui
    spinach/external/stb
	spinach/external/nanosvg
	examples/spirograph_with_rmgui
)


target_link_libraries(${SPIROGRAPH_WITH_RMGUI} PRIVATE SDL3::SDL3)


# 4. mine_sweeper
# Collect C++ source files
file(GLOB_RECURSE MINE_SWEEPER_CPP_SRC CONFIGURE_DEPENDS
    examples/mine_sweeper/*.cpp
)

# Collect headers 
file(GLOB_RECURSE MINE_SWEEPER_CPP_H CONFIGURE_DEPENDS
	examples/mine_sweeper/*.h
)

# Create the executable
add_executable(	${MINE_SWEEPER}
	${MINE_SWEEPER_CPP_SRC}
	${SPINACH_CPP_SRC}
    ${SPINACH_C_SRC}
    ${MINE_SWEEPER_CPP_H} 
	${SPINACH_H}
	${NANOSVG_H}
)
target_include_directories(${MINE_SWEEPER} PRIVATE
    spinach/common
    spinach/core
    spinach/ui
    spinach/external/stb
	spinach/external/nanosvg
	examples/mine_sweeper
)


target_link_libraries(${MINE_SWEEPER} PRIVATE SDL3::SDL3)


# 5. image_processing_with_rmgui
# Collect C++ source files
file(GLOB_RECURSE IMAGE_PROCESSING_WITH_RMGUI_CPP_SRC CONFIGURE_DEPENDS
    examples/image_processing_with_rmgui/*.cpp
)

# Collect headers 
file(GLOB_RECURSE IMAGE_PROCESSING_WITH_RMGUI_CPP_H CONFIGURE_DEPENDS
	examples/image_processing_with_rmgui/*.h
)

# Create the executable
add_executable(	${IMAGE_PROCESSING_WITH_RMGUI}
	${IMAGE_PROCESSING_WITH_RMGUI_CPP_SRC}
	${SPINACH_CPP_SRC}
    ${SPINACH_C_SRC}
    ${IMAGE_PROCESSING_WITH_RMGUI_CPP_H} 
	${SPINACH_H}
	${NANOSVG_H}
)
target_include_directories(${IMAGE_PROCESSING_WITH_RMGUI} PRIVATE
    spinach/common
    spinach/core
    spinach/ui
    spinach/external/stb
	spinach/external/nanosvg
	examples/image_processing_with_rmgui
)


target_link_libraries(${IMAGE_PROCESSING_WITH_RMGUI} PRIVATE SDL3::SDL3)


# 6. game_of_life
# Collect C++ source files
file(GLOB_RECURSE GAME_OF_LIFE_CPP_SRC CONFIGURE_DEPENDS
    examples/game_of_life/*.cpp
)

# Collect headers 
file(GLOB_RECURSE GAME_OF_LIFE_CPP_H CONFIGURE_DEPENDS
	examples/game_of_life/*.h
)

# Create the executable
add_executable(	${GAME_OF_LIFE}
	${GAME_OF_LIFE_CPP_SRC}
	${SPINACH_CPP_SRC}
    ${SPINACH_C_SRC}
    ${GAME_OF_LIFE_CPP_H} 
	${SPINACH_H}
	${NANOSVG_H}
)
target_include_directories(${GAME_OF_LIFE} PRIVATE
    spinach/common
    spinach/core
    spinach/ui
    spinach/external/stb
	spinach/external/nanosvg
	examples/game_of_life
)


target_link_libraries(${GAME_OF_LIFE} PRIVATE SDL3::SDL3)


# 6. snake_clone
# Collect C++ source files
file(GLOB_RECURSE SNAKE_CLONE_CPP_SRC CONFIGURE_DEPENDS
    examples/snake_clone/*.cpp
)

# Collect headers 
file(GLOB_RECURSE SNAKE_CLONE_CPP_H CONFIGURE_DEPENDS
	examples/snake_clone/*.h
)

# Create the executable
add_executable(	${SNAKE_CLONE}
	${SNAKE_CLONE_CPP_SRC}
	${SPINACH_CPP_SRC}
    ${SPINACH_C_SRC}
    ${SNAKE_CLONE_CPP_H} 
	${SPINACH_H}
	${NANOSVG_H}
)
target_include_directories(${SNAKE_CLONE} PRIVATE
    spinach/common
    spinach/core
    spinach/ui
    spinach/external/stb
	spinach/external/nanosvg
	examples/snake_clone
)


target_link_libraries(${SNAKE_CLONE} PRIVATE SDL3::SDL3)
