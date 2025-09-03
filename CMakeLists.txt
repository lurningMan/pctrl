cmake_minimum_required(VERSION 3.10.0)
project(pctrl VERSION 0.1.0 LANGUAGES C)

# Set output directory for all executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Include headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# First test: test_hsm
add_executable(test_hsm_basic
    test/test_hsm_basic.c
    src/hsm/hsm.c
)

# Second test: test_fsm
add_executable(test_fsm
    test/test_fsm.c
    src/fsm/fsm.c
)

# Optional: common compile warnings
foreach(target test_hsm_basic test_fsm)
    #target_compile_options(${target} PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(${target} PRIVATE)
endforeach()

