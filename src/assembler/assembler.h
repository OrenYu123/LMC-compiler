#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "../parser/parser.h"

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

struct AssembledInstruction {
    TokenType opcode;
    std::optional<int> operand;
};

class Assembler {
public:
    explicit Assembler(const std::vector<ParsedStatement>& statements);

    std::vector<AssembledInstruction> assemble();

private:
    const std::vector<ParsedStatement>& statements;

    std::unordered_map<std::string, int> labels;

    void firstPass();

    AssembledInstruction assembleStatement(
        const ParsedStatement& statement
    );

    int resolveOperand(const Token& operand) const;

    void defineLabel(
        const Token& label,
        int address
    );

    void validateDAT(
        const ParsedInstruction& instruction
    ) const;

    void throwAssemblyError(
        const std::string& message
    ) const;

    void throwAssemblyError(
        const std::string& message,
        const Token& token
    ) const;
};

#endif