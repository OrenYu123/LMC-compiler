#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/assembler/assembler.h"
#include "../src/vm/vm.h"
#include "../src/debugger/debugger.h"

#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

//----------------------
// Helper functions
//----------------------

void assertTrue(bool condition, const std::string& message){
    if(!condition){
        throw std::runtime_error(message);
    }
}

VM createVM(const std::string& source){
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);
    std::vector<AssembledInstruction> program = assembler.assemble();

    return VM(std::move(program));
}

//Runner
std::string runDebugger(VM& vm, const std::string& commands){
    std::streambuf* originalCin = std::cin.rdbuf();
    std::streambuf* originalCout = std::cout.rdbuf();

    std::stringstream input(commands);
    std::stringstream output;

    std::cin.rdbuf(input.rdbuf());
    std::cout.rdbuf(output.rdbuf());

    try{
        Debugger debugger(vm);
        debugger.run();
    }
    catch(...){
        std::cin.rdbuf(originalCin);
        std::cout.rdbuf(originalCout);
        throw;
    }
    std::cin.rdbuf(originalCin);
    std::cout.rdbuf(originalCout);
    return output.str();
}

//------------------
// Tests
//------------------

//tests a single step in the debugger
void testStep(){
    std::string source = 
    "LDA x\n"
    "HLT\n"
    "x DAT 5";
    VM vm = createVM(source);

    runDebugger(vm, "step\n""quit\n");
    assertTrue(
        vm.getAccumulator() == 5,
        "Debugger should execute one instruction."
    );
    assertTrue(
        vm.getProgramCounter() == 1,
        "Debugger step should advance the program counter."
    );
}

// Tests a step but with the abbriviatted commands
void testStepFast(){
    std::string source = 
    "LDA x\n"
    "HLT\n"
    "x DAT 5";
    VM vm = createVM(source);
    runDebugger(vm, "s\n""q\n");
    assertTrue(
        vm.getAccumulator() == 5,
        "Debugger 's' should execute one instruction."
    );
    assertTrue(
        vm.getProgramCounter() == 1,
        "Debugger 's' should advance the program counter."
    );
}

// Tests base value of registers
void testRegisters(){
    std::string source = 
    "LDA x\n"
    "HLT\n"
    "x DAT 5";
    VM vm = createVM(source);
    std::string output = runDebugger(vm, "registers\n""quit\n");
    assertTrue(
        output.find("PC: 0") != std::string::npos,
        "Registers should display the program counter."
    );
    assertTrue(
        output.find("ACC: 0") != std::string::npos,
        "Registers should display the accumulator."
    );
}

// Tests abbriviated command
void testRegistersFast(){
    std::string source = 
    "LDA x\n"
    "HLT\n"
    "x DAT 5";
    VM vm = createVM(source);
    std::string output = runDebugger(vm, "r\n""q\n");
    assertTrue(
        output.find("PC: 0") != std::string::npos,
        "Registers should display the program counter."
    );
    assertTrue(
        output.find("ACC: 0") != std::string::npos,
        "Registers should display the accumulator."
    );
}

// Tests register content after a step
void testRegistersStepped(){
    std::string source = 
    "LDA x\n"
    "HLT\n"
    "x DAT 5";
    VM vm = createVM(source);
    std::string output = runDebugger(vm, "s\n""r\n""q\n");
    assertTrue(
        output.find("PC: 1") != std::string::npos,
        "Registers should display the program counter."
    );
    assertTrue(
        output.find("ACC: 5") != std::string::npos,
        "Registers should display the accumulator."
    );
}

// Tests memory command
void testMemory(){
    std::string source = 
    "LDA x\n"
    "HLT\n"
    "x DAT 5";
    VM vm = createVM(source);

    std::string output = runDebugger(vm, "memory 2\n""q\n");

    assertTrue(
        output.find("Memory[2]: 5") != std::string::npos,
        "Memory command should display the correct value."
    );
}

// Test memory command abbreviation
void testMemoryFast(){
    std::string source = 
    "LDA x\n"
    "HLT\n"
    "x DAT 5";
    VM vm = createVM(source);

    std::string output = runDebugger(vm, "m 2\n""q\n");

    assertTrue(
        output.find("Memory[2]: 5") != std::string::npos,
        "Memory command should display the correct value."
    );
}

void testInvalidMemory(){
    std::string source = "HLT";

    VM vm = createVM(source);

    std::string output = runDebugger(vm, "m 128\nq\n");
    assertTrue(
        output.find("Error:") != std::string::npos,
        "Invalid memory address should produce an error."
    );
}

// Tests continue command
void testContinue(){
    std::string source = 
    "LDA x\n"
    "ADD y\n"
    "OUT\n"
    "HLT\n"
    "x DAT 6\n"
    "y DAT 7";
    VM vm = createVM(source);

    std::string output = runDebugger(vm, "continue\nq\n");

    assertTrue(
        output.find("13\n") != std::string::npos,
        "Continue should execute the program until it halts."
    );
    assertTrue(
        vm.isHalted(),
        "Continue should leave the VM halted."
    );
    assertTrue(
        vm.getProgramCounter() == 4,
        "Continue should leave the program counter after halt."
    );
    assertTrue(
        vm.getAccumulator() == 13,
        "Continue should leave the accumulator after halt."
    );
}

// Tests shortend command for continue
void testContinueFast(){
    std::string source = 
    "LDA x\n"
    "ADD y\n"
    "OUT\n"
    "HLT\n"
    "x DAT 6\n"
    "y DAT 7";
    VM vm = createVM(source);

    std::string output = runDebugger(vm, "c\nq\n");

    assertTrue(
        output.find("13\n") != std::string::npos,
        "Continue should execute the program until it halts."
    );
    assertTrue(
        vm.isHalted(),
        "Continue should leave the VM halted."
    );
    assertTrue(
        vm.getProgramCounter() == 4,
        "Continue should leave the program counter after halt."
    );
    assertTrue(
        vm.getAccumulator() == 13,
        "Continue should leave the accumulator after halt."
    );
}

// Tests stepping after halt
void testStepAfterHalt() {
    std::string source =
        "HLT\n"
        "LDA x\n"
        "x DAT 5";

    VM vm = createVM(source);

    runDebugger(vm, "s\ns\nq\n");

    assertTrue(
        vm.getProgramCounter() == 1,
        "Debugger step should do nothing after HLT."
    );

    assertTrue(
        vm.getAccumulator() == 0,
        "Accumulator should remain unchanged after HLT."
    );
}

// Testing continue after halt
void testContinueAfterHalt() {
    std::string source = "HLT";
    VM vm = createVM(source);

    runDebugger(vm, "s\nc\nq\n");

    assertTrue(
        vm.getProgramCounter() == 1,
        "Continue after HLT should not change the program."
    );
}

// Tests the help command
void testHelp() {
    std::string source = "HLT";
    VM vm = createVM(source);

    std::string output = runDebugger(vm, "help\nq\n");

    assertTrue(
        output.find("step") != std::string::npos,
        "Help should describe the step command."
    );

    assertTrue(
        output.find("continue") != std::string::npos,
        "Help should describe the continue command."
    );

    assertTrue(
        output.find("registers") != std::string::npos,
        "Help should describe the registers command."
    );

    assertTrue(
        output.find("memory") != std::string::npos,
        "Help should describe the memory command."
    );
}

// Test the 'h' command
void testHelpFast() {
    std::string source = "HLT";
    VM vm = createVM(source);

    std::string output = runDebugger(vm, "h\nq\n");

    assertTrue(
        output.find("Commands:") != std::string::npos,
        "Debugger 'h' should act as help."
    );
}

// Tests invalid command
void testInvalidCommand() {
    std::string source = "HLT";
    VM vm = createVM(source);

    std::string output = runDebugger(vm, "foo\nq\n");

    assertTrue(
        output.find("Unknown command: foo") != std::string::npos,
        "Invalid commands should produce an error message."
    );
}

// Test empty command
void testEmptyCommand() {
    std::string source = "HLT";
    VM vm = createVM(source);

    std::string output = runDebugger(vm, "\nq\n");

    assertTrue(
        output.find("Unknown command") == std::string::npos,
        "Empty commands should be ignored."
    );
}

// Test just quit
void testQuit() {
    std::string source = 
    "LDA x\n"
    "HLT\n"
    "x DAT 5";
    VM vm = createVM(source);

    runDebugger(vm, "q\n");

    assertTrue(
        vm.getProgramCounter() == 0,
        "Quit should exit without executing an instruction."
    );

    assertTrue(
        !vm.isHalted(),
        "Quit should not halt the VM."
    );
}

//--------------------
// Test Runner
//--------------------

int main() {
    std::vector<std::pair<std::string, std::function<void()>>> tests = {
        {"Step", testStep},
        {"Step shortened", testStepFast},
        {"Registers", testRegisters},
        {"Registers shortened", testRegistersFast},
        {"Registers after step", testRegistersStepped},
        {"Memory", testMemory},
        {"Memory shortened", testMemoryFast},
        {"Invalid memory", testInvalidMemory},
        {"Continue", testContinue},
        {"Continue shortened", testContinueFast},
        {"Step after halt", testStepAfterHalt},
        {"Continue after halt", testContinueAfterHalt},
        {"Help", testHelp},
        {"Help shortened", testHelpFast},
        {"Invalid command", testInvalidCommand},
        {"Empty command", testEmptyCommand},
        {"Quit", testQuit}
    };

    int passed = 0;
    int failed = 0;

    for (const auto& test : tests) {
        const std::string& name = test.first;
        const std::function<void()>& function = test.second;

        try {
            function();

            std::cout
                << "Passed "
                << name
                << '\n';

            passed++;
        }
        catch (const std::exception& e) {
            std::cout
                << "Failed "
                << name
                << ": "
                << e.what()
                << '\n';

            failed++;
        }
    }

    std::cout
        << "\nTests passed: "
        << passed
        << ", Tests failed: "
        << failed
        << '\n';

    return failed == 0 ? 0 : 1;
}