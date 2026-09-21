#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/assembler/assembler.h"
#include "../src/vm/vm.h"
#include "../src/common/LMC_constants.h"

#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

//--------------------
// Test Utils
//--------------------

void assertTrue(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void assertThrow(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);
    std::vector<AssembledInstruction> program = assembler.assemble();

    VM vm(program);

    bool thrown = false;

    try {
        vm.run();
    }
    catch (const std::exception&) {
        thrown = true;
    }

    assertTrue(thrown, "Expected VM to throw an error");
}

std::string runner(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);
    std::vector<AssembledInstruction> program = assembler.assemble();

    VM vm(program);

    std::streambuf* originalCout = std::cout.rdbuf();

    std::stringstream output;
    std::cout.rdbuf(output.rdbuf());

    try {
        vm.run();
    }
    catch (...) {
        std::cout.rdbuf(originalCout);
        throw;
    }

    std::cout.rdbuf(originalCout);

    return output.str();
}

//-----------------
// Batch 1
//-----------------

void testLDA() {
    std::string source =
        "LDA x\n"
        "OUT\n"
        "HLT\n"
        "x DAT 42";

    std::string output = runner(source);

    assertTrue(
        output == "42\n",
        "LDA should load value from memory"
    );
}

void testHLT() {
    std::string source =
        "LDA x\n"
        "OUT\n"
        "HLT\n"
        "LDA y\n"
        "OUT\n"
        "x DAT 5\n"
        "y DAT 1";

    std::string output = runner(source);

    assertTrue(
        output == "5\n",
        "HLT should halt the program"
    );
}

void testSTA() {
    std::string source =
        "LDA x\n"
        "STA y\n"
        "LDA y\n"
        "OUT\n"
        "HLT\n"
        "x DAT 42\n"
        "y DAT 0";

    std::string output = runner(source);

    assertTrue(
        output == "42\n",
        "STA should store the accumulator in memory"
    );
}

void testADD() {
    std::string source =
        "LDA x\n"
        "ADD y\n"
        "OUT\n"
        "HLT\n"
        "x DAT 1\n"
        "y DAT 2";

    std::string output = runner(source);

    assertTrue(
        output == "3\n",
        "ADD should add the value in memory to the accumulator"
    );
}

void testSUB() {
    std::string source =
        "LDA x\n"
        "SUB y\n"
        "OUT\n"
        "HLT\n"
        "x DAT 5\n"
        "y DAT 2";

    std::string output = runner(source);

    assertTrue(
        output == "3\n",
        "SUB should subtract the value in memory from the accumulator"
    );
}

void testADDOverflow() {
    std::string source =
        "LDA x\n"
        "ADD y\n"
        "OUT\n"
        "HLT\n"
        "x DAT " + std::to_string(LMC::MAX_VALUE) + "\n"
        "y DAT " + std::to_string(LMC::MAX_VALUE);

    std::string output = runner(source);

    assertTrue(
        output == "-2\n",
        "ADD should account for overflow"
    );
}

void testSUBOverflow() {
    std::string source =
        "LDA x\n"
        "SUB y\n"
        "OUT\n"
        "HLT\n"
        "x DAT " + std::to_string(LMC::MIN_VALUE) + "\n"
        "y DAT 1";

    std::string output = runner(source);

    assertTrue(
        output == std::to_string(LMC::MAX_VALUE) + "\n",
        "SUB should account for overflow"
    );
}

void testBRA() {
    std::string source =
        "BRA skip\n"
        "LDA x\n"
        "OUT\n"
        "skip LDA y\n"
        "OUT\n"
        "HLT\n"
        "x DAT 6\n"
        "y DAT 7";

    std::string output = runner(source);

    assertTrue(
        output == "7\n",
        "BRA should branch to the given address"
    );
}

void testBRZBranch() {
    std::string source =
        "LDA zero\n"
        "BRZ output\n"
        "LDA wrong\n"
        "OUT\n"
        "HLT\n"
        "output LDA correct\n"
        "OUT\n"
        "HLT\n"
        "zero DAT 0\n"
        "wrong DAT 10\n"
        "correct DAT 20";

    std::string output = runner(source);

    assertTrue(
        output == "20\n",
        "BRZ should branch when accumulator is zero"
    );
}

void testBRZNoBranch() {
    std::string source =
        "LDA wrong\n"
        "BRZ output\n"
        "LDA correct\n"
        "output OUT\n"
        "HLT\n"
        "correct DAT 6\n"
        "wrong DAT 7";

    std::string output = runner(source);

    assertTrue(
        output == "6\n",
        "BRZ shouldn't branch when accumulator isn't zero"
    );
}

void testBRPBranch() {
    std::string source =
        "LDA positive\n"
        "BRP output\n"
        "LDA wrong\n"
        "OUT\n"
        "HLT\n"
        "output LDA correct\n"
        "OUT\n"
        "HLT\n"
        "positive DAT 5\n"
        "wrong DAT 10\n"
        "correct DAT 20";

    std::string output = runner(source);

    assertTrue(
        output == "20\n",
        "BRP should branch when accumulator isn't negative"
    );
}

void testBRPNoBranch() {
    std::string source =
        "LDA negative\n"
        "BRP skip\n"
        "LDA correct\n"
        "OUT\n"
        "HLT\n"
        "skip LDA wrong\n"
        "OUT\n"
        "HLT\n"
        "negative DAT -1\n"
        "correct DAT 20\n"
        "wrong DAT 10";

    std::string output = runner(source);

    assertTrue(
        output == "20\n",
        "BRP shouldn't branch when accumulator is negative"
    );
}

//-----------------------
// Batch 2
//-----------------------

void testINP() {
    std::string source =
        "INP\n"
        "OUT\n"
        "HLT";

    std::streambuf* originalCin = std::cin.rdbuf();

    std::stringstream input("42\n");
    std::cin.rdbuf(input.rdbuf());

    std::string output;

    try {
        output = runner(source);
    }
    catch (...) {
        std::cin.rdbuf(originalCin);
        throw;
    }

    std::cin.rdbuf(originalCin);

    assertTrue(
        output == "42\n",
        "INP should store input in the accumulator"
    );
}

void testNegativeLDA() {
    std::string source =
        "LDA x\n"
        "OUT\n"
        "HLT\n"
        "x DAT -50";

    std::string output = runner(source);

    assertTrue(
        output == "-50\n",
        "LDA should load negative numbers within the range"
    );
}

void testBRPZero() {
    std::string source =
        "LDA zero\n"
        "BRP positive\n"
        "LDA wrong\n"
        "OUT\n"
        "HLT\n"
        "positive LDA correct\n"
        "OUT\n"
        "HLT\n"
        "zero DAT 0\n"
        "wrong DAT 10\n"
        "correct DAT 20";

    std::string output = runner(source);

    assertTrue(
        output == "20\n",
        "BRP should branch when accumulator is zero"
    );
}

void testMultADDOverflow() {
    std::string source =
        "LDA x\n"
        "ADD y\n"
        "ADD y\n"
        "OUT\n"
        "HLT\n"
        "x DAT " + std::to_string(LMC::MAX_VALUE) + "\n"
        "y DAT " + std::to_string(LMC::MAX_VALUE);

    std::string output = runner(source);

    // 127 + 127 = 254 -> -2
    // -2 + 127 = 125

    assertTrue(
        output == "125\n",
        "ADD should correctly handle multiple overflows"
    );
}

void testMultSUBOverflow() {
    std::string source =
        "LDA x\n"
        "SUB y\n"
        "SUB y\n"
        "OUT\n"
        "HLT\n"
        "x DAT " + std::to_string(LMC::MIN_VALUE) + "\n"
        "y DAT " + std::to_string(LMC::MAX_VALUE);

    std::string output = runner(source);

    // -128 - 127 = -255 -> 1
    // 1 - 127 = -126

    assertTrue(
        output == "-126\n",
        "SUB should correctly handle multiple overflows"
    );
}

void testDATExecute() {
    std::string source =
        "BRA data\n"
        "HLT\n"
        "data DAT 42";

    assertThrow(source);
}

void testPCOOB() {
    std::string source =
        "BRA " + std::to_string(LMC::MEMORY_SIZE - 1);

    assertThrow(source);
}

void testINPOverflow() {
    std::string source =
        "INP\n"
        "HLT";

    std::streambuf* originalCin = std::cin.rdbuf();

    std::stringstream input(
        std::to_string(LMC::MAX_VALUE + 1) + "\n"
    );

    std::cin.rdbuf(input.rdbuf());

    bool thrown = false;

    try {
        runner(source);
    }
    catch (const std::exception&) {
        thrown = true;
    }

    std::cin.rdbuf(originalCin);

    assertTrue(
        thrown,
        "INP should reject values outside the valid range"
    );
}

//------------------------
// Batch 3 - debug tools
//------------------------

//Helper as runner executes immediattely rather than a step
VM createVM(const std::string& source){
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);
    std::vector<AssembledInstruction> program = assembler.assemble();

    return VM(program);
}

//Tests a single step
void testStep(){
    std::string source = 
    "LDA x\n"
    "HLT\n"
    "x DAT 42";

    VM vm = createVM(source);

    vm.step();

    assertTrue(
        vm.getAccumulator()==42,
        "Step should execute one instruction."
    );
    assertTrue(
        vm.getProgramCounter()==1,
        "Program counter should advance after an instruction."
    );

}

//tests multiple steps
void testMultSteps(){
    std::string source = 
    "LDA x\n"
    "ADD y\n"
    "HLT\n"
    "x DAT 5\n"
    "y DAT 1";
    VM vm = createVM(source);
    
    vm.step();
    assertTrue(
        vm.getAccumulator()==5&&vm.getProgramCounter()==1,
        "State is incorrect after first step."
    );

    vm.step();
    assertTrue(
        vm.getAccumulator()==6&&vm.getProgramCounter()==2,
        "State is incorrect after second step."
    );
}

// Tests memory getter
void testMemoryGetter(){
    std::string source = 
    "x DAT 40\n"
    "HLT";
    VM vm = createVM(source);

    assertTrue(
        vm.getMemoryAddress(0)==40,
        "Memory address 0 should contain 40."
    );
    assertTrue(
        vm.getMemoryAddress(1)==0,
        "Memory address 1 should contain 0."
    );

}

//Tests stepping after a halt
void testStepAfterHalt(){
    std::string source = 
    "HLT\n"
    "LDA x\n"
    "x DAT 3";

    VM vm = createVM(source);

    vm.step();

    assertTrue(
        vm.getProgramCounter()==1,
        "Program counter should advance after HLT."
    );

    vm.step();

    assertTrue(
        vm.getProgramCounter()==1&&vm.getAccumulator()==0,
        "Step should do nothing after HLT."
    );
}

void testInvalidMemoryAddress(){
    VM vm = createVM("HLT");

    bool thrown = false;

    try{
        vm.getMemoryAddress(LMC::MEMORY_SIZE);
    }
    catch(const std::exception){
        thrown = true;
    }
    assertTrue(
        thrown,
        "Invalid memory address should throw an error/"
    );
}




//--------------------
// Test runner
//--------------------

int main() {
    std::vector<std::pair<std::string, std::function<void()>>> tests = {
        {"LDA", testLDA},
        {"HLT", testHLT},
        {"STA", testSTA},
        {"ADD", testADD},
        {"SUB", testSUB},
        {"ADD overflow", testADDOverflow},
        {"SUB overflow", testSUBOverflow},
        {"BRA", testBRA},
        {"BRZ", testBRZBranch},
        {"BRZ with non-zero acc", testBRZNoBranch},
        {"BRP", testBRPBranch},
        {"BRP with negative acc", testBRPNoBranch},
        {"INP", testINP},
        {"Negative LDA", testNegativeLDA},
        {"BRP with zero", testBRPZero},
        {"Multiple ADD overflow", testMultADDOverflow},
        {"Multiple SUB overflow", testMultSUBOverflow},
        {"Execute DAT", testDATExecute},
        {"OOB PC", testPCOOB},
        {"Overflow input", testINPOverflow},
        {"Step", testStep},
        {"Multiple steps", testMultSteps},
        {"Get memory address", testMemoryGetter},
        {"Get invalid memory address", testInvalidMemoryAddress},
        {"Step after HLT", testStepAfterHalt}
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