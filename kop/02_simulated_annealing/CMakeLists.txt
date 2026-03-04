cmake_minimum_required(VERSION 3.16)
project(mwsat_sa CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(main main.cpp)

target_compile_options(main PRIVATE
        $<$<CONFIG:Release>:-O3 -march=native -DNDEBUG>
)
