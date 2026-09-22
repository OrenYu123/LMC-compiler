#include "token_utils.h"

std::string tokenTypeToString(TokenType type){
    switch(type){
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

        default:
            return "UNKNOWN";
    }
}