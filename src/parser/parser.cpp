#include "parser.h"

#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens) {}

// Returns current token
const Token& Parser::current() const {
    return tokens[currentIndex];
}

// Returns next token to work on
const Token& Parser::peek() const {
    if (currentIndex + 1 < tokens.size()) {
        return tokens[currentIndex + 1];
    }
    return tokens.back();
}

// Returns current token and advances to the next one
const Token& Parser::advance() {
    const Token& token = current();
    ++currentIndex;
    return token;
}

// Parses the statements for the assembler
std::vector<ParsedStatement> Parser::parse() {
    std::vector<ParsedStatement> statements;

    while (current().type != TokenType::End_Of_File) {
        statements.push_back(parseStatement());
    }

    return statements;
}

// Parses the statement and returns if it has a label
ParsedStatement Parser::parseStatement() {
    std::optional<Token> label;

    if (current().type == TokenType::Identifier &&
        peek().line == current().line &&
        isInstruction(peek().type)) {
        label = advance();
    }

    ParsedInstruction instruction = parseInstruction();

    expectEndOfLine();

    return {
        label,
        instruction
    };
}

// Parses the instruction and checks for required operands
ParsedInstruction Parser::parseInstruction() {
    const Token& opcodeToken = current();

    if (!isInstruction(opcodeToken.type)) {
        throwSyntaxError(
            "Expected an instruction, got '" +
            opcodeToken.value +
            "'"
        );
    }

    TokenType opcode = advance().type;

    // No operands
    if (opcode == TokenType::INP ||
        opcode == TokenType::OUT ||
        opcode == TokenType::HLT) {
        return {
            opcode,
            std::nullopt
        };
    }

    // Optional operand
    if (opcode == TokenType::DAT) {
        if (current().type == TokenType::End_Of_File ||
            current().line != opcodeToken.line) {
            return {
                opcode,
                std::nullopt
            };
        }

        if (current().type != TokenType::Number) {
            throwSyntaxError("DAT expects a number or no value");
        }

        return {
            opcode,
            advance()
        };
    }

    // Requires operand
    if (isMemoryInstruction(opcode)) {
        if (current().type == TokenType::End_Of_File ||
            current().line != opcodeToken.line) {
            throwSyntaxError(
                "Instruction '" +
                opcodeToken.value +
                "' requires an operand"
            );
        }

        if (current().type != TokenType::Identifier &&
            current().type != TokenType::Number) {
            throwSyntaxError(
                "Instruction '" +
                opcodeToken.value +
                "' requires an identifier or number as an operand"
            );
        }

        return {
            opcode,
            advance()
        };
    }

    throwSyntaxError("Unknown instruction.");
    return {};
}

bool Parser::isInstruction(TokenType type) const {
    switch (type) {
        case TokenType::STA:
        case TokenType::LDA:
        case TokenType::ADD:
        case TokenType::SUB:
        case TokenType::BRA:
        case TokenType::BRP:
        case TokenType::BRZ:
        case TokenType::INP:
        case TokenType::OUT:
        case TokenType::HLT:
        case TokenType::DAT:
            return true;

        default:
            return false;
    }
}

bool Parser::isMemoryInstruction(TokenType type) const {
    switch (type) {
        case TokenType::STA:
        case TokenType::LDA:
        case TokenType::ADD:
        case TokenType::SUB:
        case TokenType::BRA:
        case TokenType::BRZ:
        case TokenType::BRP:
            return true;

        default:
            return false;
    }
}

// Checks for end of line
void Parser::expectEndOfLine() {
    if (current().type == TokenType::End_Of_File) {
        return;
    }

    if (current().line == tokens[currentIndex - 1].line) {
        throwSyntaxError(
            "Unexpected token '" +
            current().value +
            "'."
        );
    }
}

void Parser::throwSyntaxError(const std::string& message) const {
    throw std::runtime_error(
        "Syntax error at line " +
        std::to_string(current().line) +
        ", column " +
        std::to_string(current().column) +
        ": " +
        message
    );
}