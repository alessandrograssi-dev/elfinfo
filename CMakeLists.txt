cmake_minimum_required(VERSION 3.20)

project(elfinfo VERSION 0.1.0 LANGUAGES CXX)

set (SRC
    src/main.cpp
)

include_directories(include)

add_executable(elfinfo ${SRC})

target_compile_features(elfinfo PRIVATE cxx_std_17)