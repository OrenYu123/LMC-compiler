#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/assembler/assembler.h"

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

//-------------------------
// Helper functions
//-------------------------

void assertTrue(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error("Assertion Failed: " + message);
    }
}

void assertThrows(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);

    try {
        assembler.assemble();
    }
    catch (const std::runtime_error&) {
        return;
    }

    throw std::runtime_error(
        "Expected an exception to be thrown but none was"
    );
}

//-------------------
// Valid Tests
//-------------------

// Tests instructions with no operands
void testNoOperandInstructions() {
    std::string source = "INP\nOUT\nHLT";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);
    std::vector<AssembledInstruction> program = assembler.assemble();

    assertTrue(
        program.size() == 3,
        "Expected 3 assembled instructions."
    );

    assertTrue(
        program[0].opcode == TokenType::INP,
        "First instruction should be INP."
    );

    assertTrue(
        !program[0].operand.has_value(),
        "INP should not have an operand."
    );

    assertTrue(
        program[1].opcode == TokenType::OUT,
        "Second instruction should be OUT."
    );

    assertTrue(
        !program[1].operand.has_value(),
        "OUT should not have an operand."
    );

    assertTrue(
        program[2].opcode == TokenType::HLT,
        "Third instruction should be HLT."
    );

    assertTrue(
        !program[2].operand.has_value(),
        "HLT should not have an operand."
    );
}

// Tests all instructions that have operands
// (excluding DAT)
void testNumericOperandsInstructions() {
    std::string source =
        "LDA 5\n"
        "STA 20\n"
        "ADD 10\n"
        "SUB 0\n"
        "BRA 6\n"
        "BRZ 7\n"
        "BRP 8";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);
    std::vector<AssembledInstruction> program = assembler.assemble();

    assertTrue(
        program.size() == 7,
        "Expected 7 assembled instructions."
    );

    assertTrue(
        program[0].opcode == TokenType::LDA,
        "First instruction should be LDA."
    );

    assertTrue(
        program[1].opcode == TokenType::STA,
        "Second instruction should be STA."
    );

    assertTrue(
        program[2].opcode == TokenType::ADD,
        "Third instruction should be ADD."
    );

    assertTrue(
        program[3].opcode == TokenType::SUB,
        "Fourth instruction should be SUB."
    );

    assertTrue(
        program[4].opcode == TokenType::BRA,
        "Fifth instruction should be BRA."
    );

    assertTrue(
        program[5].opcode == TokenType::BRZ,
        "Sixth instruction should be BRZ."
    );

    assertTrue(
        program[6].opcode == TokenType::BRP,
        "Seventh instruction should be BRP."
    );

    assertTrue(
        program[0].operand.value() == 5,
        "LDA operand should be 5."
    );

    assertTrue(
        program[1].operand.value() == 20,
        "STA operand should be 20."
    );

    assertTrue(
        program[2].operand.value() == 10,
        "ADD operand should be 10."
    );

    assertTrue(
        program[3].operand.value() == 0,
        "SUB operand should be 0."
    );

    assertTrue(
        program[4].operand.value() == 6,
        "BRA operand should be 6."
    );

    assertTrue(
        program[5].operand.value() == 7,
        "BRZ operand should be 7."
    );

    assertTrue(
        program[6].operand.value() == 8,
        "BRP operand should be 8."
    );
}

// Tests conversion of label to address
void testLabelConversionToAddress() {
    std::string source =
        "BRA LOOP\n"
        "INP\n"
        "LOOP HLT";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);
    std::vector<AssembledInstruction> program = assembler.assemble();

    assertTrue(
        program.size() == 3,
        "Expected 3 assembled instructions."
    );

    assertTrue(
        program[0].operand.has_value(),
        "BRA should have an operand."
    );

    assertTrue(
        program[0].operand.value() == 2,
        "Label should be resolved to address 2."
    );
}

// Tests DAT
void testDat() {
    std::string source =
        "x DAT 1\n"
        "y DAT 67\n"
        "z DAT -127\n"
        "a DAT";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);
    std::vector<AssembledInstruction> program = assembler.assemble();

    assertTrue(
        program.size() == 4,
        "Expected 4 assembled instructions."
    );

    assertTrue(
        program[0].opcode == TokenType::DAT,
        "First statement should be DAT."
    );

    assertTrue(
        program[0].operand.value() == 1,
        "First DAT should contain 1."
    );

    assertTrue(
        program[1].operand.value() == 67,
        "Second DAT should contain 67."
    );

    assertTrue(
        program[2].operand.value() == -127,
        "Third DAT should contain -127."
    );

    assertTrue(
        !program[3].operand.has_value(),
        "Fourth DAT should have no operand."
    );
}

// Tests multiple labels
void testMultipleLabels() {
    std::string source =
        "BRA Start\n"
        "x DAT 5\n"
        "Start LDA x\n"
        "BRA end\n"
        "end HLT";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);
    std::vector<AssembledInstruction> program = assembler.assemble();

    assertTrue(
        program.size() == 5,
        "Expected 5 assembled instructions."
    );

    assertTrue(
        program[0].operand.value() == 2,
        "Start should be resolved to address 2."
    );

    assertTrue(
        program[2].operand.value() == 1,
        "x should be resolved to address 1."
    );

    assertTrue(
        program[3].operand.value() == 4,
        "end should be resolved to address 4."
    );
}

//-------------------------------
// Invalid Assembly Tests
//-------------------------------

// Tests undefined labels
void testUndefinedLabels() {
    std::string source = "LDA FOO";

    assertThrows(source);
}

// Tests duplicate labels
void testDupeLabels() {
    std::string source =
        "x DAT 1\n"
        "x DAT 2";

    assertThrows(source);
}

// Tests large DAT values
void testBigDAT() {
    std::string source = "x DAT 128";

    assertThrows(source);
}

// Tests small DAT values
void testSmallDAT() {
    std::string source = "x DAT -129";

    assertThrows(source);
}

// Tests invalid addresses passed as operands
void testInvalidAddresses() {
    std::vector<std::string> sources = {
        "LDA -1",
        "STA -1",
        "ADD -1",
        "SUB -1",
        "BRA -1",
        "BRZ -1",
        "BRP -1",

        "LDA 128",
        "STA 128",
        "ADD 128",
        "SUB 128",
        "BRA 128",
        "BRZ 128",
        "BRP 128"
    };

    for (const std::string& source : sources) {
        assertThrows(source);
    }
}

//----------------------------
// Complete Program Test
//----------------------------

void testCompleteProgram() {
    std::string source =
        "INP\n"
        "STA x\n"
        "loop LDA x\n"
        "SUB one\n"
        "OUT\n"
        "STA x\n"
        "BRP loop\n"
        "HLT\n"
        "x DAT\n"
        "one DAT 1";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);
    std::vector<AssembledInstruction> program = assembler.assemble();

    assertTrue(
        program.size() == 10,
        "Expected 10 assembled instructions."
    );

    assertTrue(
        program[0].opcode == TokenType::INP,
        "1st instruction should be INP."
    );

    assertTrue(
        program[1].opcode == TokenType::STA,
        "2nd instruction should be STA."
    );

    assertTrue(
        program[2].opcode == TokenType::LDA,
        "3rd instruction should be LDA."
    );

    assertTrue(
        program[3].opcode == TokenType::SUB,
        "4th instruction should be SUB."
    );

    assertTrue(
        program[4].opcode == TokenType::OUT,
        "5th instruction should be OUT."
    );

    assertTrue(
        program[5].opcode == TokenType::STA,
        "6th instruction should be STA."
    );

    assertTrue(
        program[6].opcode == TokenType::BRP,
        "7th instruction should be BRP."
    );

    assertTrue(
        program[7].opcode == TokenType::HLT,
        "8th instruction should be HLT."
    );

    assertTrue(
        program[8].opcode == TokenType::DAT,
        "9th instruction should be DAT."
    );

    assertTrue(
        program[9].opcode == TokenType::DAT,
        "10th instruction should be DAT."
    );

    assertTrue(
        !program[0].operand.has_value(),
        "INP should not have an operand."
    );

    assertTrue(
        program[1].operand.has_value(),
        "STA should have an operand."
    );

    assertTrue(
        program[1].operand.value() == 8,
        "STA x should resolve to address 8."
    );

    assertTrue(
        program[2].operand.has_value(),
        "LDA should have an operand."
    );

    assertTrue(
        program[2].operand.value() == 8,
        "LDA x should resolve to address 8."
    );

    assertTrue(
        program[3].operand.has_value(),
        "SUB should have an operand."
    );

    assertTrue(
        program[3].operand.value() == 9,
        "SUB one should resolve to address 9."
    );

    assertTrue(
        !program[4].operand.has_value(),
        "OUT should not have an operand."
    );

    assertTrue(
        program[5].operand.has_value(),
        "STA should have an operand."
    );

    assertTrue(
        program[5].operand.value() == 8,
        "STA x should resolve to address 8."
    );

    assertTrue(
        program[6].operand.has_value(),
        "BRP should have an operand."
    );

    assertTrue(
        program[6].operand.value() == 2,
        "BRP loop should resolve to address 2."
    );

    assertTrue(
        !program[7].operand.has_value(),
        "HLT should not have an operand."
    );

    assertTrue(
        !program[8].operand.has_value(),
        "x DAT should have no operand."
    );

    assertTrue(
        program[9].operand.has_value(),
        "one DAT 1 should have an operand."
    );

    assertTrue(
        program[9].operand.value() == 1,
        "one DAT 1 should contain 1."
    );
}

//----------------------------
// Batch 2 tests (edge cases)
//----------------------------

// Tests the lowest valid memory address
void testAddressZero() {
    std::string source = "LDA 0";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);
    std::vector<AssembledInstruction> program = assembler.assemble();

    assertTrue(
        program[0].operand.value() == 0,
        "Address 0 should be valid."
    );
}

// Tests the highest valid memory address
void testAddress127() {
    std::string source = "LDA 127";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);
    std::vector<AssembledInstruction> program = assembler.assemble();

    assertTrue(
        program[0].operand.value() == 127,
        "Address 127 should be valid."
    );
}

// Tests a label at address 0
void testLabelAtAddressZero() {
    std::string source =
        "start HLT\n"
        "BRA start";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);
    std::vector<AssembledInstruction> program = assembler.assemble();

    assertTrue(
        program[1].operand.value() == 0,
        "Label at address 0 should resolve to 0."
    );
}

// Tests a label at address 127
void testLabelAtAddress127() {
    std::string source;

    for (int i = 0; i < 127; i++) {
        source += "HLT\n";
    }

    source += "end HLT\n";
    source += "BRA end";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);

    assertThrows(
        source
    );
}

// Tests DAT at the minimum allowed value
void testDATMinimumValue() {
    std::string source = "x DAT -128";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);
    std::vector<AssembledInstruction> program = assembler.assemble();

    assertTrue(
        program[0].operand.value() == -128,
        "DAT should accept -128."
    );
}

// Tests DAT at the maximum allowed value
void testDATMaximumValue() {
    std::string source = "x DAT 127";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);
    std::vector<AssembledInstruction> program = assembler.assemble();

    assertTrue(
        program[0].operand.value() == 127,
        "DAT should accept 127."
    );
}

// Tests exactly 128 statements
void testMaximumProgramSize() {
    std::string source;

    for (int i = 0; i < 128; i++) {
        source += "HLT";

        if (i < 127) {
            source += '\n';
        }
    }

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);
    std::vector<AssembledInstruction> program = assembler.assemble();

    assertTrue(
        program.size() == 128,
        "A program containing exactly 128 statements should be valid."
    );
}

// Tests more than 128 statements
void testProgramTooLarge() {
    std::string source;

    for (int i = 0; i < 129; i++) {
        source += "HLT";

        if (i < 128) {
            source += '\n';
        }
    }

    assertThrows(source);
}

// Tests that multiple references to one label resolve correctly
void testMultipleReferencesToLabel() {
    std::string source =
        "BRA target\n"
        "BRA target\n"
        "LDA target\n"
        "target HLT";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);
    std::vector<AssembledInstruction> program = assembler.assemble();

    assertTrue(
        program[0].operand.value() == 3,
        "First reference should resolve to address 3."
    );

    assertTrue(
        program[1].operand.value() == 3,
        "Second reference should resolve to address 3."
    );

    assertTrue(
        program[2].operand.value() == 3,
        "Third reference should resolve to address 3."
    );
}

//------------------------------------
// Test Runner
//------------------------------------

int main() {
    int passed = 0;
    int failed = 0;

    std::vector<std::pair<std::string, std::function<void()>>> tests = {
        {"No operand instructions", testNoOperandInstructions},
        {"Numeric operand instructions", testNumericOperandsInstructions},
        {"Label conversion to address", testLabelConversionToAddress},
        {"Multiple labels", testMultipleLabels},
        {"DAT tests", testDat},

        {"Undefined labels", testUndefinedLabels},
        {"Duplicate label", testDupeLabels},
        {"Large DAT", testBigDAT},
        {"Small DAT", testSmallDAT},
        {"Invalid Addresses", testInvalidAddresses},

        {"Complete Program", testCompleteProgram},

        {"Address 0", testAddressZero},
        {"Address 127", testAddress127},
        {"Label at address 0", testLabelAtAddressZero},
        {"Label at address 127", testLabelAtAddress127},
        {"DAT minimum value", testDATMinimumValue},
        {"DAT maximum value", testDATMaximumValue},
        {"Maximum program size", testMaximumProgramSize},
        {"Program too large", testProgramTooLarge},
        {"Multiple references to label", testMultipleReferencesToLabel}
    };

    std::cout
        << "Running "
        << tests.size()
        << " assembler tests...\n";

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