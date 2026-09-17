#include "vm.h"
#include <bits/stdc++.h>

using namespace std;

VM::VM(const vector<AssembledInstruction>& program) : program(program), memory(128,0){}

void VM::run(){
    while(!hlt){
        if(pc<0 || pc >= program.size()){
            throwVMError("PC out of bounds,");
        }
        const AssembledInstruction& instruction = program[pc];
        executeInstruction(instruction);
    }
}


void VM::executeInstruction(const AssembledInstruction& instruction){
    switch (instruction.opcode)
    {
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
        pc++;
        break;
    case TokenType::BRP:
        executeBRP(*instruction.operand);
        pc++;
        break;
    case TokenType::BRZ:
        executeBRZ(*instruction.operand);
        pc++;
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
        throwVMError("Unknown Instruction");
    }
}

//Memory Instructions
void VM::executeSTA(int address){
    validateAddress(address);
    memory[address] = acc;
}
void VM::executeLDA(int address){
    validateAddress(address);
    acc = memory[address];
}
void VM::executeADD(int address){
    validateAddress(address);
    acc = wrapValue(acc + memory[address]);
}
void VM::executeSUB(int address){
    validateAddress(address);
    acc = wrapValue(acc - memory[address]);
}

//Branching Instructions
void VM::executeBRA(int address){
    validateAddress(address);
    pc = address;
}
void VM::executeBRP(int address){
    validateAddress(address);
    if(acc>=0){pc = address;}
    else pc++;
}
void VM::executeBRZ(int address){
    validateAddress(address);
    if(acc==0){pc = address;}
    else pc++;
}

//IO
void VM::executeINP(){
    cin >> acc;
}
void VM::executeOUT(){
    cout << acc << '\n';
}

//Halt
void VM::executeHLT(){hlt=true;}


//helper functions
int VM::wrapValue(int value) const{
    value %=256;
    if(value>127) value-=256;
    else if(value<-128) value+=256;
    return value;
}

void VM::validateAddress(int address) const{
    if(address < 0 || address >= static_cast<int>(memory.size())){
        throwVMError("Address must be between 0 and " + memory.size()-1 + '.');
    }
}

void VM::throwVMError(const string& message) const{
    throw runtime_error("VM error: " + message);
}