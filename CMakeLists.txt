cmake_minimum_required(VERSION 3.20)

project(LMC_Compiler
    VERSION 0.0
    DESCRIPTION "A simple compiler for the LMC (Little Man Computer) assembly language."
    )

include(CTest)
enable_testing()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Main program
add_executable(lmc
    src/main.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/assembler/assembler.cpp
    src/vm/vm.cpp
)
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

# Assembler tests
add_executable(assembler_tests
    tests/assembler_tests.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/assembler/assembler.cpp
)

# VM tests
add_executable(vm_tests
    tests/vm_tests.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/assembler/assembler.cpp
    src/vm/vm.cpp
)

# Debugger tests
add_executable(debugger_tests
    tests/debugger_tests.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/assembler/assembler.cpp
    src/vm/vm.cpp
    src/debugger/debugger.cpp
)

# Register tests with CTest
add_test(NAME LexerTests COMMAND lexer_tests)
add_test(NAME ParserTests COMMAND parser_tests)
add_test(NAME AssemblerTests COMMAND assembler_tests)
add_test(NAME VMTests COMMAND vm_tests)
add_test(NAME DebuggerTests COMMAND debugger_tests)