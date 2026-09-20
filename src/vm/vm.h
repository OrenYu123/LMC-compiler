#ifndef VM_H
#define VM_H

#include "../assembler/assembler.h"

#include <string>
#include <vector>

class VM {
public:
    explicit VM(const std::vector<AssembledInstruction>& program);

    void run();

private:
    const std::vector<AssembledInstruction>& program;

    std::vector<int> memory; // capped at 128 items
    int acc = 0;             // accumulator
    int pc = 0;              // program counter
    bool hlt = false;

    // General execute
    void executeInstruction(const AssembledInstruction& instruction);

    // Execute per instruction
    void executeSTA(int address);
    void executeLDA(int address);
    void executeADD(int address);
    void executeSUB(int address);
    void executeBRA(int address);
    void executeBRZ(int address);
    void executeBRP(int address);
    void executeINP();
    void executeOUT();
    void executeHLT();

    void initializeMemory();
    int wrapValue(int value) const;
    void validateAddress(int address) const;
    void throwVMError(const std::string& message) const;
};

#endif