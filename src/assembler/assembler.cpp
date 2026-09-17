#include "assembler.h"
#include <bits/stdc++.h>

using namespace std;

Assembler::Assembler(const vector<ParsedStatement>& statements) : statements(statements){}

vector<AssembledInstruction> Assembler::assemble(){
    firstPass();//finds each label's address

    vector<AssembledInstruction> program;
    for(const ParsedStatement& statement : statements){
        program.push_back(assembleStatement(statement));
    }
    return program;
}

void Assembler::firstPass(){
    for(int address = 0; address < statements.size(); address++){
        const ParsedStatement& statement = statements[address];
        if(statement.label.has_value()){defineLabel(*statement.label, address);}
    }
}

//checks if label exists in the dictionary and if it doesn't match the instruction's address to it
void Assembler::defineLabel(const Token& label, int address){
    if(labels.find(label.value)!=labels.end()){
        throwAssemblyError("Duplicate label '" + label.value + "'.", label);
    }
    labels[label.value] = address;
}

AssembledInstruction Assembler::assembleStatement(const ParsedStatement& statement){
    const ParsedInstruction& instruction = statement.instruction;

    if(instruction.opcode==TokenType::DAT){
        validateDAT(instruction);
        if(!instruction.operand.has_value()){
            return {instruction.opcode, nullopt};//indicates optional var doesn't have a value
        }
        return {
            instruction.opcode,
            stoi(instruction.operand->value)
        };
    }//DAT behaves differently

    if(!instruction.operand.has_value()){
        return {instruction.opcode, nullopt};//indicates optional var doesn't have a value
    }
    int operand = resolveOperand(*instruction.operand);
    return{
        instruction.opcode,
        operand
    };

}

int Assembler::resolveOperand(const Token& operand) const{
    int address;

    if(operand.type==TokenType::Number){address = stoi(operand.value);}
    else if(operand.type==TokenType::Identifier){
        auto it = labels.find(operand.value);
        if(it==labels.end()){
            throwAssemblyError("Undefined Label '" + operand.value + "'.", operand);
        }
        address = it->second;
    }
    else{throwAssemblyError("Invalid operand '" + operand.value + "'.",operand);}
    if(address<0 || address > 127){throwAssemblyError("Address must be between 0 to 127", operand);}
    return address;
}

void Assembler::validateDAT(const ParsedInstruction& instruction) const{
    if(!instruction.operand.has_value()){return;}//dat without value is valid
    const Token& value = *instruction.operand;
    int number = stoi(value.value);
    if(number < -128 || number > 127){
        throwAssemblyError("DAT value must be between -128 and 127", value);
    }
}


void Assembler::throwAssemblyError(const string& message, const Token& token) const{
    throw runtime_error("Assembly error at line " + to_string(token.line) + ", column " + to_string(token.column) + ": " + message);
}