#include "vm.h"
#include "../common/LMC_constants.h"
#include <iostream>
#include <stdexcept>
#include <utility>

VM::VM(std::vector<AssembledInstruction> program)
    : program(program), memory(LMC::MEMORY_SIZE, 0) {
    initializeMemory();
}

void VM::run() {
    while (!hlt) {
        step();
    }
}
//created for debugging
void VM::step(){
    if(hlt){return;}
    if(pc < 0 || pc >= static_cast<int>(program.size())){
        throwVMError(
            "Program counter " + 
            std::to_string(pc) + 
            " is out of bounds."
        );
    }
    const AssembledInstruction& instruction = program[pc];
    executeInstruction(instruction);
}

int VM::getAccumulator() const {
    return acc;
}

int VM::getProgramCounter() const{
    return pc;
}

bool VM::isHalted() const{
    return hlt;
}

int VM::getMemoryAddress(int address) const{
    validateAddress(address);
    return memory[address];
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
            throwVMError("Attempted to execute DAT at address " + 
                std::to_string(pc) + 
                "."
            );

        default:
            throwVMError("Unknown instruction at address " + 
            std::to_string(pc) + 
            "."
            );
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
    if(!(std::cin >> input)){
        throwVMError("Input must be an integer.");
    }

    if (input < LMC::MIN_VALUE || input > LMC::MAX_VALUE) {
        throwVMError("Input must be between " + 
            std::to_string(LMC::MIN_VALUE) + 
            " and " 
            + std::to_string(LMC::MAX_VALUE) + 
            ".");
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
    value %= LMC::VALUE_RANGE;

    if (value > LMC::MAX_VALUE) {
        value -= LMC::VALUE_RANGE;
    } else if (value < LMC::MIN_VALUE) {
        value += LMC::VALUE_RANGE;
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