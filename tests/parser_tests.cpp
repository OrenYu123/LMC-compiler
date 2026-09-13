#include <bits/stdc++.h>
#include "../src/parser/parser.h"
using namespace std;

void assertTrue(bool condition, const string& message) {
    if (!condition) {
        throw runtime_error("Assertion failed: " + message);
    }
}

void assertThrows(const string& source){
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);

    try{
        parser.parse();
    }
    catch (const runtime_error& e) {
        return; // Exception was thrown as expected
    }
    throw runtime_error("Expected an exception to be thrown, but none was.");
}

// ---------------------------------
// Valid Syntax Tests
// ---------------------------------

// Tests instructions working with memory
void testMemoryInstruction(){
    string source = "LDA x\n""STA x\n""ADD x\n""SUB x";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    parser.parse();
}

//Tests branching instructions
void testBranchInstruction(){
    string source = "BRZ label\n""BRP label\n""BRA label";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    parser.parse();
}

//Tests instructions that do not require operands
void testNoOperandInstruction(){
    string source = "INP\n""OUT\n""HLT";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    parser.parse();
}
    
//Test instructions with labels
void testLabel(){
    string source = "label LDA x";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    parser.parse();
}

//Tests the DAT instruction
void testDAT(){
    string source = "x DAT 5";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    parser.parse();
}

//Tests the DAT instruction with a negative value
void testNegativeDAT(){
    string source = "x DAT -127";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    parser.parse();
}

//Tests passing a label as an operand to an instruction
void testIdentifierOperand(){
    string source = "LDA x\n""BRA loop\n""loop HLT";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    parser.parse();
}

//Tests passing a number as an operand to an instruction
void testNumberOperand(){
    string source = "LDA 5\n""STA 20\n""BRA 10";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    parser.parse();
}


void testCompleteProgram(){
    string source = "INP\n""STA x\n""loop LDA x\n""SUB one\n""STA x\n""BRP loop\n""HLT\n""\n""x DAT 5\n""one DAT 1";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    parser.parse();
}

//Tests DAT with no init val
void testDATMissingValue(){
    string source = "x DAT";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    parser.parse();
}

//---------------------
// Invalid Syntax Tests
//---------------------

//Tests missing operand for an instruction
void testMissingOperand(){
    assertThrows("LDA");
}

//Tests missing operand for a branching instruction
void testMissingBranchOperand(){
    assertThrows("BRZ");
}

//Tests passing an operand to an instruction that does not require one
void testUnexpectedOperand(){
    assertThrows("HLT 5");
}

//Tests INP with an operand
void testINPWithOperand(){
    assertThrows("INP 5");
}

//Test OUT with an operand
void testOUTWithOperand(){
    assertThrows("OUT 5");
}

//Tests passing multiple operands to an instruction that only requires one
void testMultipleOperands(){
    assertThrows("LDA x y");
}

//Tests passing a label as an operand to a DAT instruction
void testDATWithIdentifier(){
    assertThrows("x DAT y");
}

//Tests invalid statments
void testInvalidStatement(){
    assertThrows("FOO x");
}

//Tests just labels;
void testLabelWithNoInstruction(){
    assertThrows("loop");
}

//------------------------
//Runner
//------------------------

int main(){
    int passed = 0, failed=0;

    struct Test{
        string name;
        void (*func)();
    };
    vector<Test> tests = {
        {"Memory instructions", testMemoryInstruction},
        {"Branch instructions", testBranchInstruction}, 
        {"No-operand instructions", testNoOperandInstruction}, 
        {"Labels", testLabel}, 
        {"DAT", testDAT}, 
        {"Negative DAT", testNegativeDAT}, 
        {"Identifier operands", testIdentifierOperand}, 
        {"Numeric operands", testNumberOperand}, 
        {"Complete program", testCompleteProgram}, 
        {"Missing operand", testMissingOperand}, 
        {"Missing branch operand", testMissingBranchOperand}, 
        {"Unexpected operand", testUnexpectedOperand}, 
        {"INP with operand", testINPWithOperand}, 
        {"OUT with operand", testOUTWithOperand}, 
        {"Multiple operands", testMultipleOperands}, 
        {"DAT missing value", testDATMissingValue}, 
        {"DAT with identifier", testDATWithIdentifier}, 
        {"Invalid statement", testInvalidStatement}, 
        {"Label without instruction", testLabelWithNoInstruction}
    };
    for (const auto& test : tests){
        try {
            test.func();
            cout << "Test passed: " << test.name << "." << endl;
            ++passed;
        }
        catch(const exception& e){
            cerr << "Test failed: " << test.name << ". Exception: " << e.what() << endl;
            ++failed;
        }
    }

    cout<< "\nTest passed: " << passed << ", Tests failed: " << failed << endl;
    return failed ==0 ? 0 : 1;
}


