#include "vm.h"

#include <iostream>
#include <stdexcept>

VM::VM(const std::vector<AssembledInstruction>& program)
    : program(program), memory(128, 0) {
    initializeMemory();
}

void VM::run() {
    while (!hlt) {
        if (pc < 0 || pc >= static_cast<int>(program.size())) {
            throwVMError("PC out of bounds.");
        }

        const AssembledInstruction& instruction = program[pc];
        executeInstruction(instruction);
    }
}

void VM::initializeMemory() {
    for (int address = 0;
         address < static_cast<int>(program.size());
         address++) {

        const AssembledInstruction& instruction = program[address];

        if (instruction.opcode == TokenType::DAT) {
            if (instruction.operand.has_value()) {
                memory[address] = instruction.operand.value();
            }
        }
    }
}

void VM::executeInstruction(
    const AssembledInstruction& instruction
) {
    switch (instruction.opcode) {
        case TokenType::STA:
            executeSTA(*instruction.operand);
            pc++;
            break;

        case TokenType::LDA:
            executeLDA(*instruction.operand);
            pc++;
            break;

        case TokenType::ADD:
            executeADD(*instruction.operand);
            pc++;
            break;

        case TokenType::SUB:
            executeSUB(*instruction.operand);
            pc++;
            break;

        case TokenType::BRA:
            executeBRA(*instruction.operand);
            break;

        case TokenType::BRP:
            executeBRP(*instruction.operand);
            break;

        case TokenType::BRZ:
            executeBRZ(*instruction.operand);
            break;

        case TokenType::INP:
            executeINP();
            pc++;
            break;

        case TokenType::OUT:
            executeOUT();
            pc++;
            break;

        case TokenType::HLT:
            executeHLT();
            pc++;
            break;

        case TokenType::DAT:
            throwVMError("Attempted to execute DAT");

        default:
            throwVMError("Unknown instruction.");
    }
}

// Memory instructions

void VM::executeSTA(int address) {
    validateAddress(address);
    memory[address] = acc;
}

void VM::executeLDA(int address) {
    validateAddress(address);
    acc = memory[address];
}

void VM::executeADD(int address) {
    validateAddress(address);
    acc = wrapValue(acc + memory[address]);
}

void VM::executeSUB(int address) {
    validateAddress(address);
    acc = wrapValue(acc - memory[address]);
}

// Branching instructions

void VM::executeBRA(int address) {
    validateAddress(address);
    pc = address;
}

void VM::executeBRP(int address) {
    validateAddress(address);

    if (acc >= 0) {
        pc = address;
    } else {
        pc++;
    }
}

void VM::executeBRZ(int address) {
    validateAddress(address);

    if (acc == 0) {
        pc = address;
    } else {
        pc++;
    }
}

// IO

void VM::executeINP() {
    int input;
    std::cin >> input;

    if (input < -128 || input > 127) {
        throwVMError("Input must be between -128 and 127.");
    }

    acc = input;
}

void VM::executeOUT() {
    std::cout << acc << '\n';
}

// Halt

void VM::executeHLT() {
    hlt = true;
}

// Helper functions

int VM::wrapValue(int value) const {
    value %= 256;

    if (value > 127) {
        value -= 256;
    } else if (value < -128) {
        value += 256;
    }

    return value;
}

void VM::validateAddress(int address) const {
    if (address < 0 ||
        address >= static_cast<int>(memory.size())) {

        throwVMError(
            "Address must be between 0 and " +
            std::to_string(memory.size() - 1) +
            "."
        );
    }
}

void VM::throwVMError(const std::string& message) const {
    throw std::runtime_error("VM error: " + message);
}