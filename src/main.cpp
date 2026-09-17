#include "lexer/lexer.h"
#include "parser/parser.h"
#include "assembler/assembler.h"
#include "vm/vm.h"
#include <bits/stdc++.h>

using namespace std;

int main(int args, char* argv[]) {
    
    if(args!=2){
        cerr << "Usage: little man's computer <file.lmc>\n";
        return 1;
    }
    string filename = argv[1];
    if(filename.size()<4 || filename.substr(filename.size()-4) != ".lmc"){
        cerr << "Error: input file must have an .lmc extension";
        return 1;
    }
    ifstream file(filename);
    if(!file.is_open()){
        cerr << "Error: could not open file '" << filename << ".\n";
        return 1;
    }
    string source((istreambuf_iterator<char>(file)),istreambuf_iterator<char>());
    try{
        Lexer lexer(source);
        vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        vector<ParsedStatement> statements = parser.parse();

        Assembler assembler(statements);
        vector<AssembledInstruction> program = assembler.assemble();

        VM vm(program);
        vm.run();
    }
    catch(const exception& e){
        cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}