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

    return VM(
        std::move(program),
        assembler.getLabels()
    );
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

//--------------------------------
// Batch 2 - added breakpoints
//--------------------------------



// Tests adding a breakpoint
void testBreakpoint(){
    std::string source =
        "LDA x\n"
        "ADD y\n"
        "OUT\n"
        "HLT\n"
        "x DAT 6\n"
        "y DAT 7";

    VM vm = createVM(source);

    std::string output = runDebugger(vm, "break 2\nbreakpoints\nq\n");

    assertTrue(
        output.find("Breakpoint 1 at address 2.") != std::string::npos,
        "Break should add a breakpoint at the given address."
    );
}

// Tests breakpoint abbreviation
void testBreakpointFast(){
    std::string source =
        "HLT";

    VM vm = createVM(source);

    std::string output = runDebugger(vm, "b 0\nbl\nq\n");

    assertTrue(
        output.find("Breakpoint 1 at address 0.") != std::string::npos,
        "Debugger 'b' should add a breakpoint and 'bl' should display it."
    );
}

// Tests deleting a breakpoint
void testDeleteBreakpoint(){
    std::string source =
        "HLT";

    VM vm = createVM(source);

    std::string output = runDebugger(
        vm,
        "break 0\n"
        "delete 0\n"
        "breakpoints\n"
        "q\n"
    );

    assertTrue(
        output.find("No breakpoints set.") != std::string::npos,
        "Delete should remove an existing breakpoint."
    );
}

// Tests deleting a breakpoint with the abbreviation
void testDeleteBreakpointFast(){
    std::string source =
        "HLT";

    VM vm = createVM(source);

    std::string output = runDebugger(
        vm,
        "b 0\n"
        "d 0\n"
        "bl\n"
        "q\n"
    );

    assertTrue(
        output.find("No breakpoints set.") != std::string::npos,
        "Debugger 'd' should delete a breakpoint."
    );
}

// Tests that continue stops when it reaches a breakpoint
void testContinueBreakpoint(){
    std::string source =
        "LDA x\n"
        "ADD y\n"
        "OUT\n"
        "HLT\n"
        "x DAT 6\n"
        "y DAT 7";

    VM vm = createVM(source);

    std::string output = runDebugger(
        vm,
        "break 2\n"
        "continue\n"
        "q\n"
    );

    assertTrue(
        output.find("Breakpoint hit at address 2") != std::string::npos,
        "Continue should stop when it reaches a breakpoint."
    );

    assertTrue(
        vm.getProgramCounter() == 2,
        "Program counter should be at the breakpoint."
    );

    assertTrue(
        vm.getAccumulator() == 13,
        "Accumulator should contain the value from instructions before the breakpoint."
    );

    assertTrue(
        !vm.isHalted(),
        "VM should not be halted when a breakpoint is hit."
    );
}

// Tests that the instruction at the breakpoint has not executed
void testBreakpointBeforeInstruction(){
    std::string source =
        "LDA x\n"
        "ADD y\n"
        "OUT\n"
        "HLT\n"
        "x DAT 6\n"
        "y DAT 7";

    VM vm = createVM(source);

    std::string output = runDebugger(
        vm,
        "break 2\n"
        "continue\n"
        "q\n"
    );

    assertTrue(
        output.find("\n13\n") == std::string::npos,
        "Instruction at the breakpoint should not execute."
    );

    assertTrue(
        vm.getProgramCounter() == 2,
        "Program should stop before executing the breakpoint instruction."
    );
}

// Tests that a breakpoint remains after being hit
void testBreakpointPersists(){
    std::string source =
        "LDA x\n"
        "ADD y\n"
        "OUT\n"
        "HLT\n"
        "x DAT 6\n"
        "y DAT 7";

    VM vm = createVM(source);

    std::string output = runDebugger(
        vm,
        "break 2\n"
        "continue\n"
        "breakpoints\n"
        "q\n"
    );

    assertTrue(
        output.find("Breakpoint 1 at address 2.") != std::string::npos,
        "A breakpoint should remain after being hit."
    );
}

// Tests invalid breakpoint addresses
void testInvalidBreakpoint(){
    std::string source = "HLT";

    VM vm = createVM(source);

    std::string output = runDebugger(
        vm,
        "break 128\n"
        "q\n"
    );

    assertTrue(
        output.find("Error: Inputted address is outside of range.") != std::string::npos,
        "Invalid breakpoint addresses should produce an error."
    );
}

// Tests deleting a nonexistent breakpoint
void testDeleteNonexistentBreakpoint(){
    std::string source = "HLT";

    VM vm = createVM(source);

    std::string output = runDebugger(
        vm,
        "delete 5\n"
        "q\n"
    );

    assertTrue(
        output.find("Error: Inputted address doesn't exist as a breakpoint.") != std::string::npos,
        "Deleting a nonexistent breakpoint should produce an error."
    );
}
//----------------------------------
// Batch 3 - improving commands
//----------------------------------
void testMemoryRange(){
    std::string source =
        "LDA x\n"
        "HLT\n"
        "x DAT 5";

    VM vm = createVM(source);

    std::string output = runDebugger(vm, "memory 0 2\nq\n");

    assertTrue(
        output.find("Memory[0]:") != std::string::npos,
        "Memory range should display the first address."
    );

    assertTrue(
        output.find("Memory[1]:") != std::string::npos,
        "Memory range should display the middle address."
    );

    assertTrue(
        output.find("Memory[2]: 5") != std::string::npos,
        "Memory range should display the last address."
    );
}
void testInvalidMemoryRange(){
    std::string source = "HLT";

    VM vm = createVM(source);

    std::string output = runDebugger(vm, "memory 5 2\nq\n");

    assertTrue(
        output.find("Start address must not be greater than end address.")
            != std::string::npos,
        "Reversed memory ranges should produce an error."
    );
}
void testInvalidMemoryRangeAddress(){
    std::string source = "HLT";

    VM vm = createVM(source);

    std::string output = runDebugger(vm, "memory 0 128\nq\n");

    assertTrue(
        output.find("Error: Inputted address is outside of range.")
            != std::string::npos,
        "Out-of-range memory ranges should produce an error."
    );
}

void testProgramList(){
    std::string source =
        "LDA x\n"
        "ADD y\n"
        "OUT\n"
        "HLT\n"
        "x DAT 6\n"
        "y DAT 7";

    VM vm = createVM(source);

    std::string output = runDebugger(
        vm,
        "list\n"
        "q\n"
    );
    std::cout << output;
    assertTrue(
        output.find("->0: LDA 4") != std::string::npos,
        "Program list should display the first instruction and current PC."
    );

    assertTrue(
        output.find("  1: ADD 5") != std::string::npos,
        "Program list should display ADD."
    );

    assertTrue(
        output.find("  2: OUT") != std::string::npos,
        "Program list should display OUT."
    );

    assertTrue(
        output.find("  3: HLT") != std::string::npos,
        "Program list should display HLT."
    );

    assertTrue(
        output.find("  4: DAT 6") != std::string::npos,
        "Program list should display DAT values."
    );

    assertTrue(
        output.find("  5: DAT 7") != std::string::npos,
        "Program list should display all program instructions."
    );
}

void testProgramListAfterStep(){
    std::string source =
        "LDA x\n"
        "HLT\n"
        "x DAT 5";

    VM vm = createVM(source);

    std::string output = runDebugger(
        vm,
        "step\n"
        "list\n"
        "q\n"
    );

    assertTrue(
        output.find("->1: HLT") != std::string::npos,
        "Program list should mark the current instruction."
    );
}
//------------------------------------
// Batch 4 - adding label tracking
//------------------------------------
void testLabels(){
    std::string source =
        "loop INP\n"
        "BRA loop\n";

    VM vm = createVM(source);

    const auto& labels = vm.getLabels();

    assertTrue(
        labels.find("loop") != labels.end(),
        "VM should contain assembler labels."
    );

    assertTrue(
        labels.at("loop") == 0,
        "Label should point to the correct address."
    );
}
void testBreakLabel(){
    std::string source =
        "loop INP\n"
        "BRA loop\n";

    VM vm = createVM(source);

    std::string output = runDebugger(
        vm,
        "break loop\n"
        "breakpoints\n"
        "quit\n"
    );

    assertTrue(
        output.find("Breakpoint 1 at address 0.") != std::string::npos,
        "Debugger should resolve a label when setting a breakpoint."
    );
}
void testDeleteBreakpointLabel(){
    std::string source =
        "loop INP\n"
        "BRA loop\n";

    VM vm = createVM(source);

    std::string output = runDebugger(
        vm,
        "break loop\n"
        "delete loop\n"
        "breakpoints\n"
        "quit\n"
    );

    assertTrue(
        output.find("No breakpoints set.") != std::string::npos,
        "Debugger should resolve a label when deleting a breakpoint."
    );
}
void testProgramListLabels(){
    std::string source =
        "loop INP\n"
        "BRA loop\n"
        "HLT\n";

    VM vm = createVM(source);

    std::string output = runDebugger(
        vm,
        "list\n"
        "quit\n"
    );

    assertTrue(
        output.find("->0: INP [loop]") != std::string::npos,
        "Program list should display labels."
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
        {"Quit", testQuit},
        {"Breakpoint", testBreakpoint},
        {"Breakpoint shortened", testBreakpointFast},
        {"Delete breakpoint", testDeleteBreakpoint},
        {"Delete breakpoint shortened", testDeleteBreakpointFast},
        {"Continue at breakpoint", testContinueBreakpoint},
        {"Breakpoint before instruction", testBreakpointBeforeInstruction},
        {"Breakpoint persists", testBreakpointPersists},
        {"Invalid breakpoint", testInvalidBreakpoint},
        {"Delete nonexistent breakpoint", testDeleteNonexistentBreakpoint},
        {"Memory range", testMemoryRange},
        {"Invalid memory range", testInvalidMemoryRange},
        {"Invalid memory range address", testInvalidMemoryRangeAddress},
        {"Program list", testProgramList},
        {"Program list after step", testProgramListAfterStep},
        {"Labels", testLabels},
        {"Break label", testBreakLabel},
        {"Delete breakpoint label", testDeleteBreakpointLabel},
        {"Program list labels", testProgramListLabels}
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