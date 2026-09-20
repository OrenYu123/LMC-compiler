#include "../src/lexer/lexer.h"
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// Helper function to convert TokenType enum to string for debugging purposes
std::string tokenTypeToStr(TokenType type) {
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
void assertToken(
    const Token& token,
    TokenType expectedType,
    const std::string& expectedValue
) {
    if (token.type != expectedType) {
        throw std::runtime_error(
            "Expected token type " + tokenTypeToStr(expectedType) +
            ", got " + tokenTypeToStr(token.type)
        );
    }

    if (token.value != expectedValue) {
        throw std::runtime_error(
            "Expected token value '" + expectedValue +
            "', got '" + token.value + "'"
        );
    }
}

void assertPosition(
    const Token& token,
    int expectedLine,
    int expectedColumn
) {
    if (token.line != expectedLine ||
        token.column != expectedColumn) {

        throw std::runtime_error(
            "Token line or column does not match expected position."
        );
    }
}

// -----------------------------------------------------
// Test cases for the Lexer
// -----------------------------------------------------

// Test case for instruction tokens
void testInstructions() {
    std::string source =
        "STA LDA ADD SUB BRA BRZ BRP INP OUT HLT DAT";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    std::vector<TokenType> expectedTypes = {
        TokenType::STA,
        TokenType::LDA,
        TokenType::ADD,
        TokenType::SUB,
        TokenType::BRA,
        TokenType::BRZ,
        TokenType::BRP,
        TokenType::INP,
        TokenType::OUT,
        TokenType::HLT,
        TokenType::DAT,
        TokenType::End_Of_File
    };

    if (tokens.size() != expectedTypes.size()) {
        std::cerr
            << "Test failed: Number of tokens does not match expected number."
            << std::endl;

        throw std::runtime_error(
            "Number of tokens does not match expected number."
        );
    }

    for (std::size_t i = 0; i < expectedTypes.size(); ++i) {
        if (tokens[i].type != expectedTypes[i]) {
            std::cerr
                << "Test failed: Token type mismatch at index " << i
                << ". Expected: " << tokenTypeToStr(expectedTypes[i])
                << ", Got: " << tokenTypeToStr(tokens[i].type)
                << std::endl;

            throw std::runtime_error(
                "Token type mismatch at index " +
                std::to_string(i)
            );
        }
    }

    std::cout << "Test passed: Instruction tokens." << std::endl;
}

// Test case for identifier tokens
void testIdentifier() {
    std::string source = "loop";
    Lexer lexer(source);

    std::vector<Token> tokens = lexer.tokenize();

    assertToken(tokens[0], TokenType::Identifier, "loop");
    assertToken(tokens[1], TokenType::End_Of_File, "");

    std::cout << "Test passed: Identifier token." << std::endl;
}

// Test case for multiple identifier tokens
void testMultipleIdentifiers() {
    std::string source = "var1 var2 var3";
    Lexer lexer(source);

    std::vector<Token> tokens = lexer.tokenize();

    assertToken(tokens[0], TokenType::Identifier, "var1");
    assertToken(tokens[1], TokenType::Identifier, "var2");
    assertToken(tokens[2], TokenType::Identifier, "var3");
    assertToken(tokens[3], TokenType::End_Of_File, "");

    std::cout << "Test passed: Multiple identifier tokens." << std::endl;
}

// Test case for number tokens
void testNumber() {
    std::string source = "128 0 -127";
    Lexer lexer(source);

    std::vector<Token> tokens = lexer.tokenize();

    assertToken(tokens[0], TokenType::Number, "128");
    assertToken(tokens[1], TokenType::Number, "0");
    assertToken(tokens[2], TokenType::Number, "-127");
    assertToken(tokens[3], TokenType::End_Of_File, "");

    std::cout << "Test passed: Number tokens." << std::endl;
}

// Test case for whitespace handling
void testWhitespace() {
    std::string source =
        "    STA     x\n"
        "\tLDA     y";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    assertToken(tokens[0], TokenType::STA, "STA");
    assertToken(tokens[1], TokenType::Identifier, "x");
    assertToken(tokens[2], TokenType::LDA, "LDA");
    assertToken(tokens[3], TokenType::Identifier, "y");
    assertToken(tokens[4], TokenType::End_Of_File, "");

    std::cout << "Test passed: Whitespace handling." << std::endl;
}

// Test case for comment handling
void testComments() {
    std::string source =
        "LDA x ; load x\n"
        "ADD y ; add y\n"
        "OUT";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    assertToken(tokens[0], TokenType::LDA, "LDA");
    assertToken(tokens[1], TokenType::Identifier, "x");
    assertToken(tokens[2], TokenType::ADD, "ADD");
    assertToken(tokens[3], TokenType::Identifier, "y");
    assertToken(tokens[4], TokenType::OUT, "OUT");
    assertToken(tokens[5], TokenType::End_Of_File, "");

    std::cout << "Test passed: Comment handling." << std::endl;
}

// Test case for comment only input
void testCommentOnly() {
    std::string source = ";this is just a comment";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    if (tokens.size() != 1) {
        throw std::runtime_error(
            "Comment-only input should produce exactly one token."
        );
    }

    assertToken(tokens[0], TokenType::End_Of_File, "");

    std::cout << "Test passed: Comment only." << std::endl;
}

// Test case for a complete program
void testProgram() {
    std::string source =
        "    INP\n"
        "    STA x\n"
        "    LDA y\n"
        "    ADD z\n"
        "    OUT\n"
        "    HLT\n"
        "x DAT 0\n"
        "y DAT 0\n"
        "z DAT 0";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    std::vector<TokenType> expectedTypes = {
        TokenType::INP,
        TokenType::STA,
        TokenType::Identifier,
        TokenType::LDA,
        TokenType::Identifier,
        TokenType::ADD,
        TokenType::Identifier,
        TokenType::OUT,
        TokenType::HLT,
        TokenType::Identifier,
        TokenType::DAT,
        TokenType::Number,
        TokenType::Identifier,
        TokenType::DAT,
        TokenType::Number,
        TokenType::Identifier,
        TokenType::DAT,
        TokenType::Number,
        TokenType::End_Of_File
    };

    if (tokens.size() != expectedTypes.size()) {
        std::cerr
            << "Test failed: Number of tokens does not match expected number."
            << std::endl;

        throw std::runtime_error(
            "Number of tokens does not match expected number."
        );
    }

    for (std::size_t i = 0; i < expectedTypes.size(); ++i) {
        if (tokens[i].type != expectedTypes[i]) {
            std::cerr
                << "Test failed: Token type mismatch at index " << i
                << ". Expected: " << tokenTypeToStr(expectedTypes[i])
                << ", Got: " << tokenTypeToStr(tokens[i].type)
                << std::endl;

            throw std::runtime_error(
                "Token type mismatch at index " +
                std::to_string(i)
            );
        }
    }

    std::cout << "Test passed: Complete program." << std::endl;
}

// Test case for line and column tracking
void testLineAndColumn() {
    std::string source =
        "STA x\n"
        "LDA y\n"
        "ADD z";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    assertPosition(tokens[0], 1, 1);
    assertPosition(tokens[1], 1, 5);
    assertPosition(tokens[2], 2, 1);
    assertPosition(tokens[3], 2, 5);
    assertPosition(tokens[4], 3, 1);
    assertPosition(tokens[5], 3, 5);
    assertPosition(tokens[6], 3, 6);

    std::cout << "Test passed: Line and column tracking." << std::endl;
}

// Test case for invalid character handling
void testInvalidCharacter() {
    std::string source =
        "STA x\n"
        "LDA y\n"
        "ADD z\n"
        "@";

    Lexer lexer(source);

    try {
        lexer.tokenize();
    }
    catch (const std::runtime_error&) {
        std::cout << "Test passed: Invalid character handling."
                  << std::endl;
        return;
    }

    throw std::runtime_error(
        "Expected exception for invalid character, but none was thrown."
    );
}

// Test case for empty input
void testEmptyInput() {
    std::string source = "";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    if (tokens.size() != 1 ||
        tokens[0].type != TokenType::End_Of_File) {

        throw std::runtime_error(
            "Empty input should produce exactly one End_Of_File token."
        );
    }

    std::cout << "Test passed: Empty input." << std::endl;
}

// Test case for identifier characters
void testIdentifierCharacters() {
    std::string source = "_foo foo_bar var123";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    assertToken(tokens[0], TokenType::Identifier, "_foo");
    assertToken(tokens[1], TokenType::Identifier, "foo_bar");
    assertToken(tokens[2], TokenType::Identifier, "var123");
    assertToken(tokens[3], TokenType::End_Of_File, "");

    std::cout << "Test passed: Identifier characters." << std::endl;
}

// -----------------------------------------------------
// Test runner
// -----------------------------------------------------

int main() {
    int passed = 0;
    int failed = 0;

    struct Test {
        std::string name;
        void (*func)();
    };

    std::vector<Test> tests = {
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

    for (const auto& test : tests) {
        try {
            test.func();
            ++passed;
        }
        catch (const std::exception& e) {
            std::cerr
                << "Test failed: "
                << test.name
                << ". Exception: "
                << e.what()
                << std::endl;

            ++failed;
        }
    }

    std::cout
        << "Tests passed: "
        << passed
        << ", Tests failed: "
        << failed
        << std::endl;

    return failed == 0 ? 0 : 1;
}

