#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "../parser/parser.h"
#include <bits/stdc++.h>

using namespace std;

struct AssembledInstruction{
    TokenType opcode;
    optional<int> operand;
};

class Assembler{
    public: 
        explicit Assembler(const vector<ParsedStatement>& statements);
        vector<AssembledInstruction> assemble();
    private:
        const vector<ParsedStatement>& statements;

        unordered_map<string, int> labels;//maps label to position

        void firstPass();
        AssembledInstruction assembleStatement(const ParsedStatement& statement);
        int resolveOperand(const Token& operand) const;
        void defineLabel(const Token& label, int address);
        void validateDAT(const ParsedInstruction& instruction) const;

        void throwAssemblyError(
            const string& message,
            const Token& token
        ) const;

};

#endif