#include "lexer/lexer.h"
#include "parser/parser.h"
#include "assembler/assembler.h"
#include "vm/vm.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    
    if(argc!=2){
        std::cerr << "Usage: little man's computer <file.lmc>\n";
        return 1;
    }
    std::string filename = argv[1];
    if(filename.size()<4 || filename.substr(filename.size()-4) != ".lmc"){
        std::cerr << "Error: input file must have an .lmc extension";
        return 1;
    }
    std::ifstream file(filename);
    if(!file.is_open()){
        std::cerr << "Error: could not open file '" << filename << ".\n";
        return 1;
    }
    std::string source((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
    try{
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        std::vector<ParsedStatement> statements = parser.parse();

        Assembler assembler(statements);
        std::vector<AssembledInstruction> program = assembler.assemble();

        VM vm(program);
        vm.run();
    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}