cmake_minimum_required(VERSION 3.16)
project(dk1_pc LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(dk1_core
    src/reset_state.c
)
target_include_directories(dk1_core PUBLIC include)

if(MSVC)
    target_compile_options(dk1_core PRIVATE /W4)
else()
    target_compile_options(dk1_core PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(dk1_pc src/main.c)
target_link_libraries(dk1_pc PRIVATE dk1_core)

enable_testing()
add_executable(test_reset tests/test_reset.c)
target_link_libraries(test_reset PRIVATE dk1_core)
add_test(NAME reset_state COMMAND test_reset)
