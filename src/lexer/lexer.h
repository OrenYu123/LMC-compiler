#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <cstddef>

enum class TokenType {
    Identifier,
    Number,

    STA,
    LDA,
    ADD,
    SUB,
    BRA,
    BRZ,
    BRP,
    INP,
    OUT,
    HLT,
    DAT,

    End_Of_File,
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

class Lexer {
public:
    explicit Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string source;
    std::size_t currentIndex = 0;
    int currentLine = 1;
    int currentColumn = 1;

    char current() const;
    char peek() const;
    void advance();

    void skipComments();
    void skipWhitespace();

    Token readWord();
    Token readNumber();

    TokenType keywordType(const std::string& word) const;
};

#endif