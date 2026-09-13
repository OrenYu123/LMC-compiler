#ifndef LEXER_H
#define LEXER_H
#include <bits/stdc++.h>
using namespace std;

enum class TokenType {
    // Define your token types here
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
    //Tokens for the lexer to use (lexical analysis)
    TokenType type;
    string value;
    int line;
    int column;

};

class Lexer {//interface for the lexer class
    public:
        explicit Lexer(const string& source);//explicit constructor to prevent implicit conversions (string to lexer)
        vector<Token> tokenize();
    private:
        string source;
        size_t currentIndex=0;
        int currentLine=1;
        int currentColumn=1;

        // Helper functions for tokenization
        char current() const;
        char peek() const;
        void advance();

        // Whitespace and comment handling functions
        void skipComments();
        void skipWhitespace();

        //Token reading functions
        Token readWord();
        Token readNumber();

        TokenType keywordType(const string& word) const;
};
#endif