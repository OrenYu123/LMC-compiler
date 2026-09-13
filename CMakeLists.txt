cmake_minimum_required(VERSION 3.20)

project(LMC_Compiler
    VERSION 0.0
    DESCRIPTION "A simple compiler for the LMC (Little Man Computer) assembly language."
    )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Main program
# add_executable(miniasm
#     src/main.cpp
#     src/lexer/lexer.cpp
# )

# Lexer tests
add_executable(lexer_tests
    tests/lexer_tests.cpp
    src/lexer/lexer.cpp
)

# Parser tests
add_executable(parser_tests
    tests/parser_tests.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
)