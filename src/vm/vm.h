#ifndef VM_H
#define VM_H

#include "../assembler/assembler.h"

#include <string>
#include <vector>
#include <unordered_map>

class VM {
public:
    explicit VM(std::vector<AssembledInstruction> program);
    VM(
        std::vector<AssembledInstruction> program,
        std::unordered_map<std::string, int> labels
    );
    void run();
    void step();
    int getAccumulator() const;
    int getProgramCounter() const;
    bool isHalted() const;
    int getMemoryAddress(int address) const;
    int getProgramSize() const;
    const std::unordered_map<std::string, int>& getLabels() const;

    const AssembledInstruction& getInstruction(int address) const;

private:
    std::vector<AssembledInstruction> program;

    std::vector<int> memory; // capped at 128 items
    int acc = 0;             // accumulator
    int pc = 0;              // program counter
    bool hlt = false;
    std::unordered_map<std::string, int> labels;

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
    [[noreturn]]void throwVMError(const std::string& message) const;
};

#endif