cmake_minimum_required(VERSION 3.13)
project(Scanner)

set(CMAKE_CXX_STANDARD 17)

set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Wextra -Wconversion -pedantic -Werror=return-type")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

add_executable(Scanner main.cpp Scanner.h Token.h config.h SymbolTable.h Exception.h StringLiteralPool.h Compiler.h Scanner.cpp FileUtil.h Exception.cpp FileUtil.cpp Compiler.cpp Token.cpp Parser.h Parser.cpp)