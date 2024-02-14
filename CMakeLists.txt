cmake_minimum_required(VERSION 3.0)

project(Compiladores)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 17)

#set(CMAKE_C_COMPILER "clang")

include_directories(src)

# Get all the source files
file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")
file(GLOB_RECURSE C_SOURCES "src/*.c")

# Bison and Flex
find_package(BISON)
find_package(FLEX)
find_package(Boost REQUIRED)

include_directories(Parser ${CMAKE_CURRENT_BINARY_DIR})
BISON_TARGET(Parser rules/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.h)
FLEX_TARGET(Scanner rules/scanner.l ${CMAKE_CURRENT_BINARY_DIR}/scanner.c)
ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)


add_executable(Compiladores ${CPP_SOURCES} ${C_SOURCES} ${BISON_Parser_OUTPUTS} ${FLEX_Scanner_OUTPUTS}
        )
