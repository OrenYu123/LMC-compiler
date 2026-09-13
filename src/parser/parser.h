#ifndef PARSER_H//incase of mult definitons
#define PARSER_H
#include "../lexer/lexer.h"
#include <bits/stdc++.h>

using namespace std;

//struct to use for instructions
struct ParsedInstruction{
    TokenType opcode;
    optional<Token> operand;
};

//struct to use for label + instruction
struct ParsedStatement{
    optional<Token> label;
    ParsedInstruction instruction;
};

class Parser{
    public:
        explicit Parser(const vector<Token>& tokens);
        vector<ParsedStatement> parse();
    private:
        const vector<Token>& tokens;
        size_t currentIndex = 0;
        const Token& current() const;//So caller can't modify it (where ends in const)
        const Token& peek() const;
        const Token& advance();

        ParsedStatement parseStatement();
        ParsedInstruction parseInstruction();

        bool isInstruction(TokenType type) const;
        bool isMemoryInstruction(TokenType type) const;

        void expectEndOfLine();
        void throwSyntaxError(const string& message) const;


};

#endif