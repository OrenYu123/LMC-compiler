#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "../vm/vm.h"

#include <string>
class Debugger{
    public:
        Debugger(VM& vm);

        void run();
    private:
        VM& vm;
        bool running;

        void executeCommand(const std::string& input);

        void step();
        void continueExecution();
        void printRegisters();
        void printMemory(int address);
        void printHelp();
};
#endif