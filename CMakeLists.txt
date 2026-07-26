cmake_minimum_required(VERSION 3.14)
project(MyParserApp CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Add cppparser directory
add_subdirectory(external/cppparser)

# 2. Create your executable
add_executable(StructParser StructParser.cpp)

# 3. Explicitly tell CMake where the header files live
target_include_directories(StructParser PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external
    ${CMAKE_CURRENT_SOURCE_DIR}/external/common
    ${CMAKE_CURRENT_SOURCE_DIR}/external/cppparser/cppast/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/cppparser/cppparser/include
)

# 4. Link the compiled binaries
target_link_libraries(StructParser PRIVATE cppparser)