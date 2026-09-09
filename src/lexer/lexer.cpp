#include "lexer.h"
#include <bits/stdc++.h>

Lexer::Lexer(const string& source) : source(source) {}

char Lexer::current() const {
    if (currentIndex < source.size()) {
        return source[currentIndex];
    }
    return '\0'; // Return null character if out of bounds
}

char Lexer::peek() const {
    if (currentIndex + 1 < source.size()) {
        return source[currentIndex + 1];
    }
    return '\0'; // Return null character if out of bounds
}

void Lexer::advance() {
    if(current() == '\n') {
        currentLine++;
        currentColumn = 1;
    } else {
        currentColumn++;
    }
    currentIndex++;
}

void Lexer::skipWhitespace(){
    while(isspace(static_cast<unsigned char>(current()))) {
        advance();
    }
}

void Lexer::skipComments(){
    while(current()!='\n'&& current()!='\0'){
        advance();
    }
}

Token Lexer::readWord(){
    const int startLine = currentLine;
    const int startColumn = currentColumn;
    string word;
    while(isalnum(static_cast<unsigned char>(current())) || current() == '_') {
        word += current();
        advance();
    }
    return{
        keywordType(word),
        word,
        startLine,
        startColumn
    };
}

Token Lexer::readNumber(){
    const int startLine = currentLine;
    const int startColumn = currentColumn;
    string number;

    if(current() == '-'){
        number += current();
        advance();
    }

    while(isdigit(static_cast<unsigned char>(current()))) {
        number += current();
        advance();
    }
    return{
        TokenType::Number,
        number,
        startLine,
        startColumn
    };
}

TokenType Lexer::keywordType(const string& word) const {
    static const unordered_map<string, TokenType> keywords = {
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
    return TokenType::Identifier; // Default to Identifier if not a keyword
}

vector<Token> Lexer::tokenize() {
    vector<Token> tokens;
    while (current() != '\0') {
        skipWhitespace();
        if (current() == '\0') break;

        if (isalpha(static_cast<unsigned char>(current())) || current() == '_') {// Identifiers and keywords start with a letter or underscore
            tokens.push_back(readWord());
        } else if (isdigit(static_cast<unsigned char>(current()))|| (current() == '-' && isdigit(static_cast<unsigned char>(peek())))) {// Numbers can start with a digit or a negative sign followed by a digit
            tokens.push_back(readNumber());
        } else if (current() == ';') { // ';' starts a comment
            skipComments();
        } else {
            // Handle unexpected characters or symbols
            throw runtime_error("Unexpected character: " + string(1, current()) + " at line " + to_string(currentLine) + ", column " + to_string(currentColumn));
        }
    }
    tokens.push_back({TokenType::End_Of_File, "", currentLine, currentColumn});
    return tokens;
}