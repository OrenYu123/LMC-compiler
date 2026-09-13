#include "parser.h"
#include <bits/stdc++.h>

using namespace std;

Parser::Parser(const vector<Token>& tokens) : tokens(tokens){}

//returns current index
const Token& Parser::current() const{
    return tokens[currentIndex];
}

//returns next token to work on
const Token& Parser::peek() const{
    if(currentIndex +1 < tokens.size()){
        return tokens[currentIndex+1];
    }
    return tokens.back();
}

//returns current token and advances to the next one
const Token& Parser::advance(){
    const Token& token = current();
    ++currentIndex;
    return token;
}

//parses the statements for the assembler
vector<ParsedStatement> Parser::parse(){
    vector<ParsedStatement> statements;
    while(current().type != TokenType::End_Of_File){
        statements.push_back(parseStatement());
    }
    return statements;
}

//Parses the statement and returns if it has a label
ParsedStatement Parser::parseStatement(){
    optional<Token> label;
    if(current().type == TokenType::Identifier && peek().line == current().line && isInstruction(peek().type)){
        label = advance();
    }
    ParsedInstruction instruction = parseInstruction();
    expectEndOfLine();
    return{
        label,
        instruction
    };
}

//Parses the instruction and checks for required operands
ParsedInstruction Parser::parseInstruction(){
    const Token& opcodeToken = current();
    if(!isInstruction(opcodeToken.type)){
        throwSyntaxError("Expected an instruction, got '" + opcodeToken.value + "'");
    }
    TokenType opcode = advance().type;
    //no operands
    if(opcode==TokenType::INP||
        opcode==TokenType::OUT||
        opcode==TokenType::HLT
    ){
        return {
            opcode,
            nullopt
        };
    }
    //optional operand
    if(opcode==TokenType::DAT){
        if(current().type == TokenType::End_Of_File || current().line != opcodeToken.line){
            return{
                opcode,
                nullopt
            };
        }
        if(current().type != TokenType::Number){
            throwSyntaxError("Dat expects a number or no value");
        }
        return {
            opcode,
            advance()
        };
    }
    //requires operand
    if(isMemoryInstruction(opcode)){
        if(current().type == TokenType::End_Of_File || current().line != opcodeToken.line){
            throwSyntaxError("Instruction '" + opcodeToken.value + "' requires an operand");
        }
        if(current().type != TokenType::Identifier && current().type!= TokenType::Number){
            throwSyntaxError("Instruction '" + opcodeToken.value + "' requires an identifier or number as an operand");
        }
        return{
            opcode,
            advance()
        };
    }
    throwSyntaxError("Unknown instruction.");
    return {};
}


bool Parser::isInstruction(TokenType type) const{
    switch (type)
    {
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

bool Parser::isMemoryInstruction(TokenType type) const{
    switch(type){
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

//checks for endofline 
void Parser::expectEndOfLine(){
    if(current().type == TokenType::End_Of_File){return;}
    if(current().line == tokens[currentIndex-1].line){
        throwSyntaxError("Unexpected token '" + current().value + "'.");
    }
}

void Parser::throwSyntaxError(const string& message) const{
    throw runtime_error(
        "Syntax error at line " + to_string(current().line) + ", column " + to_string(current().column) + ": " + message
    );
}

