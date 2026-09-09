#include "../src/lexer/lexer.h"
#include <bits/stdc++.h>
using namespace std;

//helper function to convert TokenType enum to string for debugging purposes
string tokenTypeToStr(TokenType type) {
    switch (type) {
        case TokenType::Identifier: return "Identifier";
        case TokenType::Number: return "Number";
        case TokenType::STA: return "STA";
        case TokenType::LDA: return "LDA";
        case TokenType::ADD: return "ADD";
        case TokenType::SUB: return "SUB";
        case TokenType::BRA: return "BRA";
        case TokenType::BRZ: return "BRZ";
        case TokenType::BRP: return "BRP";
        case TokenType::INP: return "INP";
        case TokenType::OUT: return "OUT";
        case TokenType::HLT: return "HLT";
        case TokenType::DAT: return "DAT";
        case TokenType::End_Of_File: return "End_Of_File";
        default: return "Unknown";
    }
}

// Helper function to assert that a token matches the expected type and value
void assertToken(const Token& token, TokenType expectedType, const string& expectedValue){
    if(token.type != expectedType){
        throw runtime_error(
            "Expected token type " + tokenTypeToStr(expectedType) +
            ", got " + tokenTypeToStr(token.type)
        );
    }
    if(token.value != expectedValue){
        throw runtime_error("Expected token value '" + expectedValue + "', got '" + token.value + "'");
    }
}

void assertPosition(const Token& token, int expectedLine, int expectedColumn){
    if(token.line != expectedLine || token.column != expectedColumn){
        throw runtime_error("Token line or column does not match expected position.");
    }
}

// -----------------------------------------------------
// Test cases for the Lexer
// -----------------------------------------------------

// Test case for instruction tokens
void testInstructions(){
    string source = "STA LDA ADD SUB BRA BRZ BRP INP OUT HLT DAT";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();

    vector<TokenType> expectedTypes = {
        TokenType::STA, TokenType::LDA, TokenType::ADD, TokenType::SUB,
        TokenType::BRA, TokenType::BRZ, TokenType::BRP, TokenType::INP,
        TokenType::OUT, TokenType::HLT, TokenType::DAT, TokenType::End_Of_File
    };
    if(tokens.size() != expectedTypes.size()){
        cerr << "Test failed: Number of tokens does not match expected number." << endl;
        throw runtime_error("Number of tokens does not match expected number.");
    }
    for(size_t i = 0; i < expectedTypes.size(); ++i){
       if(tokens[i].type != expectedTypes[i]){
            cerr << "Test failed: Token type mismatch at index " << i
                 << ". Expected: " << tokenTypeToStr(expectedTypes[i])
                 << ", Got: " << tokenTypeToStr(tokens[i].type) << endl;
            throw runtime_error("Token type mismatch at index " + to_string(i));
        }
    }
    cout << "Test passed: Instruction tokens." << endl;
}

// Test case for identifier tokens
void testIdentifier(){
    string source = "loop";
    Lexer lexer(source);

    vector<Token> tokens = lexer.tokenize();
    assertToken(tokens[0], TokenType::Identifier, "loop");
    assertToken(tokens[1], TokenType::End_Of_File, "");
    cout << "Test passed: Identifier token." << endl;
}

// Test case for multiple identifier tokens
void testMultipleIdentifiers(){
    string source = "var1 var2 var3";
    Lexer lexer(source);

    vector<Token> tokens = lexer.tokenize();
    assertToken(tokens[0], TokenType::Identifier, "var1");
    assertToken(tokens[1], TokenType::Identifier, "var2");
    assertToken(tokens[2], TokenType::Identifier, "var3");
    assertToken(tokens[3], TokenType::End_Of_File, "");
    cout << "Test passed: Multiple identifier tokens." << endl;
}

// Test case for number tokens
void testNumber(){
    string source = "128 0 -127";
    Lexer lexer(source);

    vector<Token> tokens = lexer.tokenize();
    assertToken(tokens[0], TokenType::Number, "128");
    assertToken(tokens[1], TokenType::Number, "0");
    assertToken(tokens[2], TokenType::Number, "-127");
    assertToken(tokens[3], TokenType::End_Of_File, "");
    cout << "Test passed: Number tokens." << endl;
}

// Test case for whitespace handling
void testWhitespace(){
    string source = "    STA     x\n""\tLDA     y";
    Lexer lexer(source);
    vector <Token> tokens = lexer.tokenize();
    assertToken(tokens[0], TokenType::STA, "STA");
    assertToken(tokens[1], TokenType::Identifier, "x");
    assertToken(tokens[2], TokenType::LDA, "LDA");
    assertToken(tokens[3], TokenType::Identifier, "y");
    assertToken(tokens[4], TokenType::End_Of_File, "");
    cout << "Test passed: Whitespace handling." << endl;
}

// Test case for comment handling
void testComments(){
    string source = "LDA x ; load x\n"
        "ADD y ; add y\n"
        "OUT";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();

    assertToken(tokens[0], TokenType::LDA, "LDA");
    assertToken(tokens[1], TokenType::Identifier, "x");
    assertToken(tokens[2], TokenType::ADD, "ADD");
    assertToken(tokens[3], TokenType::Identifier, "y");
    assertToken(tokens[4], TokenType::OUT, "OUT");
    assertToken(tokens[5], TokenType::End_Of_File, "");
    cout << "Test passed: Comment handling." << endl;

}

// Test case for comment only input
void testCommentOnly(){
    string source = ";this is just a comment";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    if(tokens.size() != 1){
        throw runtime_error("Comment-only input should produce exactly one token.");
    }
    assertToken(tokens[0], TokenType::End_Of_File, "");
    cout << "Test passed: Comment only." << endl;
}

// Test case for a complete program
void testProgram(){
    string source = "    INP\n"
                    "    STA x\n"
                    "    LDA y\n"
                    "    ADD z\n"
                    "    OUT\n"
                    "    HLT\n"
                    "x DAT 0\n"
                    "y DAT 0\n"
                    "z DAT 0";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    vector<TokenType> expectedTypes = {
        TokenType::INP, TokenType::STA, TokenType::Identifier,
        TokenType::LDA, TokenType::Identifier,
        TokenType::ADD, TokenType::Identifier,
        TokenType::OUT, TokenType::HLT,
        TokenType::Identifier, TokenType::DAT, TokenType::Number,
        TokenType::Identifier, TokenType::DAT, TokenType::Number,
        TokenType::Identifier, TokenType::DAT, TokenType::Number,
        TokenType::End_Of_File
    };
    if(tokens.size() != expectedTypes.size()){
        cerr << "Test failed: Number of tokens does not match expected number." << endl;
        throw runtime_error("Number of tokens does not match expected number.");
    }
    for(size_t i = 0; i < expectedTypes.size(); ++i){
       if(tokens[i].type != expectedTypes[i]){
            cerr << "Test failed: Token type mismatch at index " << i
                 << ". Expected: " << tokenTypeToStr(expectedTypes[i])
                 << ", Got: " << tokenTypeToStr(tokens[i].type) << endl;
            throw runtime_error("Token type mismatch at index " + to_string(i));
        }
    }
    cout << "Test passed: Complete program." << endl;
}

// Test case for line and column tracking
void testLineAndColumn(){
    string source = "STA x\n"
                    "LDA y\n"
                    "ADD z";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();

    assertPosition(tokens[0], 1, 1);
    assertPosition(tokens[1], 1, 5);
    assertPosition(tokens[2], 2, 1);
    assertPosition(tokens[3], 2, 5);
    assertPosition(tokens[4], 3, 1);
    assertPosition(tokens[5], 3, 5);
    assertPosition(tokens[6], 3, 6); // End_Of_File token position
    cout << "Test passed: Line and column tracking." << endl;
}

// Test case for invalid character handling
void testInvalidCharacter(){
    string source = "STA x\n"
                    "LDA y\n"
                    "ADD z\n"
                    "@"; // Invalid character
    Lexer lexer(source);
    try {
        vector<Token> tokens = lexer.tokenize();
    } catch (const runtime_error& e) {
        cout << "Test passed: Invalid character handling. Caught exception: " << e.what() << endl;
        return; // Test passed, exit the function
    }
    throw runtime_error("Expected exception for invalid character, but none was thrown."); // If we reach here, the test failed
}

// Test case for empty input
void testEmptyInput(){
    string source = "";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    if(tokens.size() != 1 || tokens[0].type != TokenType::End_Of_File){
        throw runtime_error("Empty input should produce exactly one End_Of_File token.");
    }
    cout << "Test passed: Empty input." << endl;
}

// Test case for identifier characters
void testIdentifierCharacters(){
    string source = "_foo foo_bar var123";
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    assertToken(tokens[0], TokenType::Identifier, "_foo");
    assertToken(tokens[1], TokenType::Identifier, "foo_bar");
    assertToken(tokens[2], TokenType::Identifier, "var123");
    assertToken(tokens[3], TokenType::End_Of_File, "");
    cout << "Test passed: Identifier characters." << endl;
}

//------------------------------------------------------
//Test runner
//------------------------------------------------------

int main() {
    int passed = 0;
    int failed = 0;
    struct test{
        string name;
        void (*func)();
    };
    vector<test> tests = {
        {"Instruction tokens", testInstructions},
        {"Identifier token", testIdentifier},
        {"Multiple identifier tokens", testMultipleIdentifiers},
        {"Number tokens", testNumber},
        {"Whitespace handling", testWhitespace},
        {"Comment handling", testComments},
        {"Complete program", testProgram},
        {"Line and column tracking", testLineAndColumn},
        {"Invalid character handling", testInvalidCharacter},
        {"Empty input", testEmptyInput},
        {"Comment only", testCommentOnly},
        {"Identifier characters", testIdentifierCharacters}
    };
    for(const auto& t : tests){
        try{
            t.func();
            ++passed;
        } catch (const exception& e){
            cerr << "Test failed: " << t.name << ". Exception: " << e.what() << endl;
            ++failed;
        }
    }
    cout << "Tests passed: " << passed << ", Tests failed: " << failed << endl;
    return failed == 0 ? 0 : 1; // Return 0 if all tests passed, otherwise return 1
}