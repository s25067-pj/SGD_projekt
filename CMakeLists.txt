cmake_minimum_required(VERSION 3.24)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

set(SDL2_INCLUDE_DIR "C:\\Users\\debis\\Downloads\\SDL2-devel-2.30.1-VC\\SDL2-2.30.1\\include")
set(SDL2_LIB_DIR "C:\\Users\\debis\\Downloads\\SDL2-devel-2.30.1-VC\\SDL2-2.30.1\\lib\\x64")

include_directories(${SDL2_INCLUDE_DIR})
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIB_DIR}/SDL2.lib ${SDL2_LIB_DIR}/SDL2main.lib)