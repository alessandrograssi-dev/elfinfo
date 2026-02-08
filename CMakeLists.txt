cmake_minimum_required(VERSION 3.20)

project(elfinfo VERSION 0.1.0 LANGUAGES CXX)

set (SRC
    src/main.cpp
    src/ElfParser.cpp
)

include_directories(include)

add_executable(elfinfo ${SRC})

target_compile_features(elfinfo PRIVATE cxx_std_17)

# Debug flags
target_compile_options(elfinfo PRIVATE
    $<$<CONFIG:Debug>: -Wall -Wextra -Wpedantic -g>
    $<$<CONFIG:Release>: -O3 >
)