#include <bits/stdc++.h>
#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/assembler/assembler.h"

using namespace std;

//-------------------------
// Helper functions
//-------------------------

void assertTrue(bool condition, const string& message){
    if(!condition){throw runtime_error("Assertion Failed: " + message);}
}

void assertThrows(const string& source){
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);

    try {
        assembler.assemble();
    }
    catch(const runtime_error& e){
        return;
    }
    throw runtime_error("Expected an exception to be thrown but none was");
}

//-------------------
// Valid Tests
//-------------------


//Tests instructions with no operands
void testNoOperandInstructions(){
    string source = "INP\nOUT\nHLT";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    vector<ParsedStatement> statements = parser.parse();
    Assembler assembler(statements);
    vector<AssembledInstruction> program = assembler.assemble();

    assertTrue(program.size() == 3, "Expected 3 assembled instructions.");
    assertTrue(program[0].opcode == TokenType::INP, "First instruction should be INP.");
    assertTrue(!program[0].operand.has_value(), "INP should not have an operand.");
    assertTrue(program[1].opcode == TokenType::OUT, "Second instruction should be OUT.");
    assertTrue(!program[1].operand.has_value(), "OUT should not have an operand.");
    assertTrue(program[2].opcode == TokenType::HLT, "Third instruction should be HLT.");
    assertTrue(!program[2].operand.has_value(), "HLT should not have an operand.");

}


//Tests all instructions that have operand(Excluding DAT).
void testNumericOperandsInstructions(){
    string source = "LDA 5\nSTA 20\nADD 10\nSUB 0\nBRA 6\nBRZ 7\nBRP 8";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    vector<ParsedStatement> statements = parser.parse();
    Assembler assembler(statements);
    vector<AssembledInstruction> program = assembler.assemble();

    assertTrue(program.size()==7, "Expected 7 assembled instructions");
    assertTrue(program[0].opcode == TokenType::LDA, "First instruction should be LDA");
    assertTrue(program[0].operand.value() == 5, "LDA operand should be 5");
    assertTrue(program[1].opcode == TokenType::STA, "Second instruction should be STA");
    assertTrue(program[2].opcode == TokenType::ADD, "Third instruction should be ADD");
    assertTrue(program[3].opcode == TokenType::SUB, "Fourth instruction should be SUB");
    assertTrue(program[4].opcode == TokenType::BRA, "Fifth instruction should be BRA");
    assertTrue(program[5].opcode == TokenType::BRZ, "Sixth instruction should be BRZ");
    assertTrue(program[6].opcode == TokenType::BRP, "Seventh instruction should be BRP");
    assertTrue(program[1].operand.value() == 20, "STA operand should be 20");
    assertTrue(program[2].operand.value() == 10, "ADD operand should be 10");
    assertTrue(program[3].operand.value() == 0, "SUB operand should be 0");
    assertTrue(program[4].operand.value() == 6, "BRA operand should be 6");//Branching instructions can have either label or memory address.
    assertTrue(program[5].operand.value() == 7, "BRZ operand should be 7");
    assertTrue(program[6].operand.value() == 8, "BRP operand should be 8");

}

//Tests the conversion of label to address
void testLabelConversionToAddress(){
    string source = "BRA LOOP\nINP\nLOOP HLT";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    vector<ParsedStatement> statements = parser.parse();
    Assembler assembler(statements);
    vector<AssembledInstruction> program = assembler.assemble();

    assertTrue(program.size()==3, "Expected 3 assembled instructions");
    assertTrue(program[0].operand.has_value(), "BRA should have an operand");
    assertTrue(program[0].operand.value() == 2, "Label should be resolved to 2");

}

//Tests DAT 
void testDat(){
    string source = "x DAT 1\n y DAT 67\n z DAT -127\n a DAT";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    vector<ParsedStatement> statements = parser.parse();
    Assembler assembler(statements);
    vector<AssembledInstruction> program = assembler.assemble();

    assertTrue(program.size() == 4, "Expected 4 assembled instructions");
    assertTrue(program[0].opcode == TokenType::DAT, "First statement should be DAT.");
    assertTrue(program[0].operand.value() == 1, "First DAT should contain 5.");
    assertTrue(program[1].operand.value() == 67, "Second DAT should contain -127.");
    assertTrue(program[2].operand.value() == -127,   "Third DAT should contain 127.");
    assertTrue(!program[3].operand.has_value(), "Fourth DAT should have no operand");
}

// Tests multiple labels
void testMultipleLabels(){
    string source = "BRA Start\nx DAT 5\nStart LDA x\nBRA end\nend HLT";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    vector<ParsedStatement> statements = parser.parse();
    Assembler assembler(statements);
    vector<AssembledInstruction> program = assembler.assemble();

    assertTrue(program.size() == 5, "Expected 5 assembled instructions");
    assertTrue(program[0].operand.value() == 2, "Label should be resolved to 2");
    assertTrue(program[2].operand.value() == 1, "Label should be resolved to 1");
    assertTrue(program[3].operand.value() == 4, "Label should resolve to 4");

}

//-------------------------------
// Invalid Assembly Tests
//-------------------------------

// Tests undefined labels
void testUndefinedLabels(){
    string source = "LDA FOO";
    assertThrows(source);
}

// Tests duplicate labels
void testDupeLabels(){
    string source = "x DAT 1\nx DAT 2";
    assertThrows(source);
}

//Tests for large DAT values (x >= 128)
void testBigDAT(){
    string source = "x DAT 128";
    assertThrows(source);
}

//Tests for small DAT values (x <= -129)
void testSmallDAT(){
    string source = "x DAT -129";
    assertThrows(source);
}

//Tests for invalid addresses passed as operands
void testInvalidAddresses(){
    vector<string> sources = {
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
    for(const string& source : sources){assertThrows(source);}
}

//----------------------------
// Complete Program Test
//----------------------------

void testCompleteProgram(){
    string source = 
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
    vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    vector<ParsedStatement> statements = parser.parse();
    Assembler assembler(statements);
    vector<AssembledInstruction> program = assembler.assemble();
    
    assertTrue(program.size()==10, "Expected 10 assembled instructions");
    assertTrue(program[0].opcode == TokenType::INP, "1st instruction should be INP");
    assertTrue(program[1].opcode == TokenType::STA, "2nd instruction should be STA");
    assertTrue(program[2].opcode == TokenType::LDA, "3rd instruction should be LDA");
    assertTrue(program[3].opcode == TokenType::SUB, "4th instruction should be SUB");
    assertTrue(program[4].opcode == TokenType::OUT, "5th instruction should be OUT");
    assertTrue(program[5].opcode == TokenType::STA, "6th instruction should be STA");
    assertTrue(program[6].opcode == TokenType::BRP, "7th instruction should be BRP");
    assertTrue(program[7].opcode == TokenType::HLT, "8th instruction should be HLT");
    assertTrue(program[8].opcode == TokenType::DAT, "9th instruction should be DAT");
    assertTrue(program[9].opcode == TokenType::DAT, "10th instruction should be DAT");

    assertTrue(!program[0].operand.has_value(), "INP should not have an operand");
    assertTrue(program[1].operand.has_value(),  "STA should have an operand");
    assertTrue(program[1].operand.value() == 8, "STA x should resolve to address 8");
    assertTrue(program[2].operand.has_value(),  "LDA should have an operand");
    assertTrue(program[2].operand.value() == 8, "LDA x should resolve to address 8");
    assertTrue(program[3].operand.has_value(),  "SUB should have an operand");
    assertTrue(program[3].operand.value() == 9, "SUB one should resolve to address 9");
    assertTrue(!program[4].operand.has_value(), "OUT should not have an operand");
    assertTrue(program[5].operand.has_value(),  "STA should have an operand");
    assertTrue(program[5].operand.value() == 8, "STA x should resolve to address 8");
    assertTrue(program[6].operand.has_value(),  "BRP should have an operand");
    assertTrue(program[6].operand.value() == 2, "BRP loop should resolve to address 2");
    assertTrue(!program[7].operand.has_value(), "HLT should not have an operand");
    assertTrue(!program[8].operand.has_value(), "x DAT should have no operand");
    assertTrue(program[9].operand.has_value(),  "one DAT 1 should have an operand");
    assertTrue(program[9].operand.value() == 1, "one DAT 1 should contain 1");
}

//------------------------------------
//Test Runner
//------------------------------------

int main(){
    int passed = 0;
    int failed = 0;

    vector<pair<string, function<void()>>> tests = {
        {"No operand instructions", testNoOperandInstructions},
        {"Numeric operand instructions", testNumericOperandsInstructions},
        {"Label conversion to address", testLabelConversionToAddress},
        {"Multiple labels", testMultipleLabels},
        {"DAT tests", testDat},
        {"Undefined labels", testUndefinedLabels},
        {"Duplicate label", testDupeLabels},
        {"Large DAT", testBigDAT},
        {"Small DAT", testSmallDAT},
        {"Complete Program", testCompleteProgram},
        {"Invalid Addresses", testInvalidAddresses}
    };
    cout << "Running " << tests.size() << " assembler tests...\n";
    for(const auto& test : tests){
        const string& name = test.first;
        const function<void()>& fun = test.second;
        try{
            fun();
            cout << "Passed " << name << '\n';
            passed++;
        }
        catch(const exception& e){
            cout << "Failed " << name << '\n           ' << e.what() << '\n';
            failed++;
        }
        
    }
    cout<< "\nTest passed: " << passed << ", Tests failed: " << failed << endl;
    return failed ==0 ? 0 : 1;

}
