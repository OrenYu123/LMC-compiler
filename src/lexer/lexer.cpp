#include "lexer.h"

#include <cctype>
#include <stdexcept>
#include <unordered_map>

Lexer::Lexer(const std::string& source) : source(source) {}

char Lexer::current() const {
    if (currentIndex < source.size()) {
        return source[currentIndex];
    }
    return '\0';
}

char Lexer::peek() const {
    if (currentIndex + 1 < source.size()) {
        return source[currentIndex + 1];
    }
    return '\0';
}

void Lexer::advance() {
    if (current() == '\n') {
        currentLine++;
        currentColumn = 1;
    } else {
        currentColumn++;
    }
    currentIndex++;
}

void Lexer::skipWhitespace() {
    while (std::isspace(static_cast<unsigned char>(current()))) {
        advance();
    }
}

void Lexer::skipComments() {
    while (current() != '\n' && current() != '\0') {
        advance();
    }
}

Token Lexer::readWord() {
    const int startLine = currentLine;
    const int startColumn = currentColumn;
    std::string word;

    while (std::isalnum(static_cast<unsigned char>(current())) ||
           current() == '_') {
        word += current();
        advance();
    }

    return {
        keywordType(word),
        word,
        startLine,
        startColumn
    };
}

Token Lexer::readNumber() {
    const int startLine = currentLine;
    const int startColumn = currentColumn;
    std::string number;

    if (current() == '-') {
        number += current();
        advance();
    }

    while (std::isdigit(static_cast<unsigned char>(current()))) {
        number += current();
        advance();
    }

    return {
        TokenType::Number,
        number,
        startLine,
        startColumn
    };
}

TokenType Lexer::keywordType(const std::string& word) const {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"STA", TokenType::STA},
        {"LDA", TokenType::LDA},
        {"ADD", TokenType::ADD},
        {"SUB", TokenType::SUB},
        {"BRA", TokenType::BRA},
        {"BRZ", TokenType::BRZ},
        {"BRP", TokenType::BRP},
        {"INP", TokenType::INP},
        {"OUT", TokenType::OUT},
        {"HLT", TokenType::HLT},
        {"DAT", TokenType::DAT}
    };

    auto it = keywords.find(word);

    if (it != keywords.end()) {
        return it->second;
    }

    return TokenType::Identifier;
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (current() != '\0') {
        skipWhitespace();

        if (current() == '\0') {
            break;
        }

        if (std::isalpha(static_cast<unsigned char>(current())) ||
            current() == '_') {
            tokens.push_back(readWord());

        } else if (
            std::isdigit(static_cast<unsigned char>(current())) ||
            (current() == '-' &&
             std::isdigit(static_cast<unsigned char>(peek())))
        ) {
            tokens.push_back(readNumber());

        } else if (current() == ';') {
            skipComments();

        } else {
            throw std::runtime_error(
                "Unexpected character: " +
                std::string(1, current()) +
                " at line " +
                std::to_string(currentLine) +
                ", column " +
                std::to_string(currentColumn)
            );
        }
    }

    tokens.push_back({
        TokenType::End_Of_File,
        "",
        currentLine,
        currentColumn
    });

    return tokens;
}