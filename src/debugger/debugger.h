#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "../vm/vm.h"
#include <unordered_set>
#include <string>
class Debugger{
    public:
        Debugger(VM& vm);

        void run();
    private:
        VM& vm;
        bool running;
        std::unordered_set<int> breakpoints;

        void executeCommand(const std::string& input);

        void step();
        void continueExecution();
        void printRegisters();
        void printMemory(int address);
        void printHelp();

        void addBreakpoint(int address);
        void removeBreakpoint(int address);
        void printBreakpoints();
};
#endif