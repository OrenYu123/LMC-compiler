#include "assembler.h"

#include <stdexcept>
#include <string>

Assembler::Assembler(
    const std::vector<ParsedStatement>& statements
)
    : statements(statements) {}

std::vector<AssembledInstruction> Assembler::assemble() {
    firstPass();

    std::vector<AssembledInstruction> program;

    for (const ParsedStatement& statement : statements) {
        program.push_back(assembleStatement(statement));
    }

    return program;
}

void Assembler::firstPass() {
    if (statements.size() > 128) {
        throwAssemblyError(
            "Program cannot contain more than 128 statements."
        );
    }

    for (std::size_t address = 0; address < statements.size(); address++) {
        const ParsedStatement& statement = statements[address];

        if (statement.label.has_value()) {
            defineLabel(*statement.label, address);
        }
    }
}

// Checks if label exists in the dictionary and, if it doesn't,
// matches the instruction's address to it
void Assembler::defineLabel(const Token& label, int address) {
    if (labels.find(label.value) != labels.end()) {
        throwAssemblyError(
            "Duplicate label '" + label.value + "'.",
            label
        );
    }

    labels[label.value] = address;
}

AssembledInstruction Assembler::assembleStatement(
    const ParsedStatement& statement
) {
    const ParsedInstruction& instruction = statement.instruction;

    if (instruction.opcode == TokenType::DAT) {
        validateDAT(instruction);

        if (!instruction.operand.has_value()) {
            return {
                instruction.opcode,
                std::nullopt
            };
        }

        return {
            instruction.opcode,
            std::stoi(instruction.operand->value)
        };
    }

    // Instructions without operands
    if (!instruction.operand.has_value()) {
        return {
            instruction.opcode,
            std::nullopt
        };
    }

    int operand = resolveOperand(*instruction.operand);

    return {
        instruction.opcode,
        operand
    };
}

int Assembler::resolveOperand(const Token& operand) const {
    int address;

    if (operand.type == TokenType::Number) {
        address = std::stoi(operand.value);
    }
    else if (operand.type == TokenType::Identifier) {
        auto it = labels.find(operand.value);

        if (it == labels.end()) {
            throwAssemblyError(
                "Undefined label '" + operand.value + "'.",
                operand
            );
        }

        address = it->second;
    }
    else {
        throwAssemblyError(
            "Invalid operand '" + operand.value + "'.",
            operand
        );
    }

    if (address < 0 || address > 127) {
        throwAssemblyError(
            "Address must be between 0 and 127.",
            operand
        );
    }

    return address;
}

void Assembler::validateDAT(
    const ParsedInstruction& instruction
) const {
    if (!instruction.operand.has_value()) {
        return;
    }

    const Token& value = *instruction.operand;
    int number = std::stoi(value.value);

    if (number < -128 || number > 127) {
        throwAssemblyError(
            "DAT value must be between -128 and 127.",
            value
        );
    }
}

void Assembler::throwAssemblyError(
    const std::string& message,
    const Token& token
) const {
    throw std::runtime_error(
        "Assembly error at line " +
        std::to_string(token.line) +
        ", column " +
        std::to_string(token.column) +
        ": " +
        message
    );
}

void Assembler::throwAssemblyError(
    const std::string& message
) const {
    throw std::runtime_error(
        "Assembly error: " + message
    );
}