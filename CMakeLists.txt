cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(Compiler)

set(CMAKE_CXX_STANDARD 17)

set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Wextra -Wconversion -pedantic -Werror=return-type")

# for release :
set(CMAKE_CXX_FLAGS "-O3")
# for debug :
set(CMAKE_CXX_FLAGS_DEBUG "-O0")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${GCC_COVERAGE_COMPILE_FLAGS}")

# find boost library
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.65.1)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(Compiler main.cpp Scanner.h Token.h config.h SymbolTable.h Exception.h
        StringLiteralPool.h Compiler.h Scanner.cpp FileUtil.h Exception.cpp FileUtil.cpp
        Compiler.cpp Token.cpp Parser.h Parser.cpp Util.h Util.cpp Analyser.h Analyser.cpp
            CodeGen.h CodeGen.cpp TypeSystem.h Code.h)
    target_link_libraries(Compiler ${Boost_LIBRARIES})
endif()