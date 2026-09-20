#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"

#include <optional>
#include <string>
#include <vector>
#include <cstddef>

struct ParsedInstruction {
    TokenType opcode;
    std::optional<Token> operand;
};

struct ParsedStatement {
    std::optional<Token> label;
    ParsedInstruction instruction;
};

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::vector<ParsedStatement> parse();

private:
    const std::vector<Token>& tokens;
    std::size_t currentIndex = 0;

    const Token& current() const;
    const Token& peek() const;
    const Token& advance();

    ParsedStatement parseStatement();
    ParsedInstruction parseInstruction();

    bool isInstruction(TokenType type) const;
    bool isMemoryInstruction(TokenType type) const;

    void expectEndOfLine();
    void throwSyntaxError(const std::string& message) const;
};

#endif