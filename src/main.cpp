#include "lexer/lexer.h"
#include "parser/parser.h"
#include "assembler/assembler.h"
#include "vm/vm.h"
#include "debugger/debugger.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string(argv[1]) == "--help") {
        std::cout << "Little Man's Computer\n\n";
        std::cout << "Usage:\n";
        std::cout << "  lmc <file.lmc>            Run a program\n";
        std::cout << "  lmc <file.lmc> --debug    Run a program in the debugger\n";
        std::cout << "  lmc --help                Display this help message\n";
        return 0;
    }
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: lmc <file.lmc> [--debug]\n";
        return 1;
    }

    std::string filename = argv[1];

    if (filename.size() < 4 ||
        filename.substr(filename.size() - 4) != ".lmc") {

        std::cerr << "Error: input file must have an .lmc extension.\n";
        return 1;
    }

    bool debug = false;

    if (argc == 3) {
        if (std::string(argv[2]) == "--debug") {
            debug = true;
        }
        else {
            std::cerr << "Error: unknown option '" << argv[2] << "'.\n";
            std::cerr << "Usage: lmc <file.lmc> [--debug]\n";
            return 1;
        }
    }

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: could not open file '" << filename << "'.\n";
        return 1;
    }

    std::string source(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    try {
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        std::vector<ParsedStatement> statements = parser.parse();

        Assembler assembler(statements);
        std::vector<AssembledInstruction> program = assembler.assemble();

        VM vm(std::move(program), assembler.getLabels());

        if (debug) {
            Debugger debugger(vm);
            debugger.run();
        }
        else {
            vm.run();
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}

