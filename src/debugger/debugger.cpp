#include "debugger.h"
#include "../common/LMC_constants.h"
#include <iostream>
#include <sstream>

Debugger::Debugger(VM& vm) : vm(vm), running(true) {}

void Debugger::run(){
    std::cout << "Little Man's Computer Debugger\n";
    std::cout << "Type help for a list of commands\n";

    printRegisters();

    std::string input;
    while(running){
        std::cout << "\n(debug)";

        if(!std::getline(std::cin, input)){
            break;
        }
        executeCommand(input);
    }
}

void Debugger::executeCommand(const std::string& input){
    std::stringstream ss(input);

    std::string command;
    ss >> command;

    if(command=="step" || command == "s"){step();}
    else if(command == "continue" || command == "c"){continueExecution();}
    else if(command == "registers" || command == "r"){printRegisters();}
    else if(command == "memory" || command == "m"){
        int address;
        if(!(ss >> address)){
            std::cout << "Usage: memory <address>\n";
            return;
        }
        printMemory(address);
    }
    else if(command == "help" || command == "h"){
        printHelp();
    }
    else if(command == "quit" || command == "q"){
        running = false;
    }
    else if(command == "break" || command == "b"){
        int address;
        if(!(ss >> address)){
            std::cout << "Usage: break <address>\n";
            return;
        }
        addBreakpoint(address);
    }
    else if(command =="delete" || command == "d"){
         int address;
         if(!(ss >> address)){
            std::cout << "Usage: delete <address>\n";
            return;
         }
         removeBreakpoint(address);
    }
    else if(command == "breakpoints" || command == "bl"){
        printBreakpoints();
    }
    else if(command.empty()){
        //ignore
    }
    else{
        std::cout << "Unknown command: " + command << "\n";
        std::cout << "Type 'help' for a list of commands.\n";
    }
}

void Debugger::step(){
    if(vm.isHalted()){
        std::cout << "Program is already halted.\n";
        return;
    }
    try{
        vm.step();
        printRegisters();
    }
    catch(const std::exception& e){
        std::cout << "Error: " << e.what() << '\n';
        return;
    }
}

void Debugger::continueExecution(){
    if(vm.isHalted()){
        std::cout << "Program is already halted.\n";
        return;   
    }

    try{
        while(!vm.isHalted()){
            if(breakpoints.find(vm.getProgramCounter())!=breakpoints.end()){
                std::cout << "Breakpoint hit at address " << vm.getProgramCounter() << ".\n";
                printRegisters();
                return;
            }
            vm.step();
        }
        std::cout << "Program Halted.\n";
        printRegisters();
    }
    catch (const std::exception& e){
        std::cout << "Error: " << e.what() << "\n";
    }
}

void Debugger::printRegisters(){
    std::cout << "PC: " << vm.getProgramCounter() << '\n';
    std::cout << "ACC: " << vm.getAccumulator() << '\n';
}

void Debugger::printHelp(){
    std::cout << "\nCommands:\n" <<
    "  step, s\n" << "    Execute one instruction.\n\n" <<
    "  continue, c\n" << "      Execute until the program halts.\n\n" <<
    "  registers, r\n" << "      Displays the program counter and accumulator.\n\n" <<
    "  memory <address>, m <address>\n" << "      Displays a memory location.\n\n" <<
    "  break <address>, b <address>\n" << "      Adds a breakpoint at the address.\n\n" <<
    "  delete <address>, d <address>\n" << "      Deletes a breakpoint at the address.\n\n" <<
    "  breakpoints, bl\n" << "      Displays the existing breakpoints.\n\n" <<
    "  help, h\n" << "      Displays this help message.\n\n" <<
    "  quit, q\n" << "      Exits the debugger.\n\n";
}

void Debugger::printMemory(int address){
    try{
        std::cout << "Memory[" << address << "]: "
                  << vm.getMemoryAddress(address) << '\n';
    }
    catch(const std::exception& e){
        std::cout << "Error: " << e.what() << '\n';
    }
}

void Debugger::addBreakpoint(int address){
    if(address<0 || address >= LMC::MEMORY_SIZE){
        std::cout << "Error: Inputted address is outside of range.\n";
        return;
    }
    breakpoints.insert(address);
}

void Debugger::removeBreakpoint(int address){
    if(address<0 || address >= LMC::MEMORY_SIZE){
        std::cout << "Error: Inputted address is outside of range.\n";
        return;
    }
    auto it = breakpoints.find(address);
    if(it!=breakpoints.end()){
        breakpoints.erase(address);
    }
    else{
        std::cout << "Error: Inputted address doesn't exist as a breakpoint.\n";
    }
}

void Debugger::printBreakpoints(){
    if(breakpoints.empty()){
        std::cout << "No breakpoints set.\n";
        return;
    }
    int i = 1;
    for(const auto& address : breakpoints){
        std::cout << "Breakpoint " << i << " at address " << address << ".\n";
        i++;
    }
}