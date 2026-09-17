#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/assembler/assembler.h"
#include "../src/vm/vm.h"

#include <bits/stdc++.h>

using namespace std;

//--------------------
//Test Utils
//--------------------


void assertTrue(bool condition, const string& message){
    if(!condition){
        throw runtime_error(message);
    }
}

void assertThrow(const string& source){
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);
    vector<AssembledInstruction> program = assembler.assemble();

    VM vm(program);

    bool thrown = false;

    try{
        vm.run();
    }
    catch(const exception&){
        thrown = true;
    }
    assertTrue(thrown, "Expected VM to throw an error");
}

string runner(const string& source){
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    vector<ParsedStatement> statements = parser.parse();

    Assembler assembler(statements);
    vector<AssembledInstruction> program = assembler.assemble();

    VM vm(program);

    streambuf* originalCout = cout.rdbuf();
    stringstream output;
    cout.rdbuf(output.rdbuf());//stores IO in the output stream
    try{
        vm.run();
    }
    catch(...){
        cout.rdbuf(originalCout);
        throw;
    }
    cout.rdbuf(originalCout);

    return output.str();
}

//-----------------
// Batch 1
//-----------------
void testLDA(){
    string source = "LDA x\n""OUT\n""HLT\n""x DAT 42";
    string output = runner(source);
    assertTrue(output=="42\n", "LDA should load value from memory");
}

void testHLT(){
    string source = "LDA x\nOUT\nHLT\nLDA y\nOUT\nx DAT 5\ny DAT 1";
    string output = runner(source);
    assertTrue(output=="5\n","HLT should halt the program");
}

void testSTA(){
    string source = "LDA x\n""STA y\n""LDA y\n""OUT\n""HLT\n""x DAT 42\n""y DAT 0";

    string output = runner(source);
    assertTrue(output == "42\n", "STA should store the acc in memory");
}

void testADD(){
    string source = "LDA x\nADD y\nOUT\nHLT\nx DAT 1\ny DAT 2";
    string output = runner(source);
    assertTrue(output=="3\n","ADD should add the value in memory to the acc");
}

void testSUB(){
    string source = "LDA x\nSUB y\nOUT\nHLT\nx DAT 5\ny DAT 2";
    string output = runner(source);
    assertTrue(output=="3\n","SUB should subtract the value in memory to the acc");
}

void testADDOverflow(){
    string source = "LDA x\nADD y\nOUT\nHLT\nx DAT 67\ny DAT 69";
    string output = runner(source);
    assertTrue(output=="-120\n","ADD should account for overflow");
}

void testSUBOverflow(){
    string source = "LDA x\nSUB y\nOUT\nHLT\nx DAT -120\ny DAT 9";
    string output = runner(source);
    assertTrue(output=="127\n","SUB should account for overflow");
}

void testBRA(){
    string source = "BRA skip\nLDA x\nOUT\nskip LDA y\nOUT\nHLT\nx DAT 6\ny DAT 7";
    string output = runner(source);
    assertTrue(output=="7\n", "BRA should branch to the given address");
}
void testBRZBranch(){
    string source =
        "LDA zero\nBRZ output\nLDA wrong\nOUT\nHLT\noutput LDA correct\nOUT\nHLT\nzero DAT 0\nwrong DAT 10\ncorrect DAT 20";
    string output = runner(source);
    assertTrue(output == "20\n","BRZ should branch when accumulator is zero");
}
void testBRZNoBranch(){
    string source = "LDA wrong\nBRZ output\nLDA correct\noutput OUT\nHLT\ncorrect DAT 6\nwrong DAT 7";
    string output = runner(source);
    assertTrue(output=="6\n", "BRZ shouldn't branch when acc isn't zero");
}
void testBRPBranch(){
    string source = "LDA positive\nBRP output\nLDA wrong\nOUT\nHLT\noutput LDA correct\nOUT\nHLT\npositive DAT 5\nwrong DAT 10\ncorrect DAT 20";
    string output = runner(source);
    assertTrue(output=="20\n", "BRP should branch when acc isn't negative");
}
void testBRPNoBranch(){
    string source = "LDA negative\nBRP skip\nLDA correct\nOUT\nHLT\nskip LDA wrong\nOUT\nHLT\nnegative DAT -1\ncorrect DAT 20\nwrong DAT 10";
    string output = runner(source);
    assertTrue(output=="20\n", "BRP shouldn't branch when acc is negative");
}

//-----------------------
// Batch 2
//-----------------------

void testINP(){
    string source = "INP\nOUT\nHLT";
    streambuf* originalCin = cin.rdbuf();
    stringstream input("42\n");
    cin.rdbuf(input.rdbuf());
    string output = runner(source);
    cin.rdbuf(originalCin);
    assertTrue(output == "42\n","INP should store input in the accumulator");
}

void testNegativeLDA(){
    string source = "LDA x\nOUT\nHLT\nx DAT -50";
    string output = runner(source);
    assertTrue(output=="-50\n","LDA should load negative numbers within the range");
}

void testBRPZero(){
    string source = "LDA zero\nBRP positive\nLDA wrong\nOUT\nHLT\npositive LDA correct\nOUT\nHLT\nzero DAT 0\nwrong DAT 10\ncorrect DAT 20";
    string output = runner(source);
    assertTrue(output=="20\n", "BRP should branch when acc is zero");
}

void testMultADDOverflow(){
    string source =
        "LDA x\nADD y\nADD y\nOUT\nHLT\nx DAT 127\ny DAT 127";
    string output = runner(source);
    // 127 + 127 = 254 -> -2
    // -2 + 127 = 125
    assertTrue(output == "125\n","ADD should correctly handle multiple overflows");
}

void testMultSUBOverflow(){
    string source =
        "LDA x\nSUB y\nSUB y\nOUT\nHLT\nx DAT -128\ny DAT 127";
    string output = runner(source);
    // -128 - 127 = -255 -> 1
    // 1 - 127 = -126
    assertTrue(output == "-126\n","SUB should correctly handle multiple overflows");
}

void testDATExecute(){
    string source = "BRA data\nHLT\ndata DAT 42";
    assertThrow(source);
}

void testPCOOB(){
    string source = "BRA 127";
    assertThrow(source);
}

void testINPOverflow(){
    string source = "INP\nHLT";
    streambuf* originalCin = cin.rdbuf();
    stringstream input("200\n");
    cin.rdbuf(input.rdbuf());
    bool thrown = false;
    try{
        runner(source);
    }
    catch(const exception&){
        thrown = true;
    }
    cin.rdbuf(originalCin);
    assertTrue(thrown, "INP should reject values above 127 and below -128");
}

//--------------------
//Test runner
//--------------------

int main(){
    vector<pair<string, function<void()>>> tests = {
        {"LDA", testLDA},
        {"HLT", testHLT},
        {"ADD", testADD},
        {"SUB", testSUB},
        {"ADD overflow", testADDOverflow},
        {"SUB overflow", testSUBOverflow},
        {"BRA", testBRA},
        {"BRZ", testBRZBranch},
        {"BRZ with non-zero acc", testBRZNoBranch},
        {"BRP", testBRPBranch},
        {"BRP with negative acc", testBRPNoBranch},
        {"INP", testINP},
        {"Negative LDA", testNegativeLDA},
        {"BRP with zero", testBRPZero},
        {"Multiple ADD overflow", testMultADDOverflow},
        {"Multiple SUB overflow", testMultSUBOverflow},
        {"Execute DAT", testDATExecute},
        {"OOB PC", testPCOOB},
        {"Overflown input", testINPOverflow}
    };
    int passed = 0;
    int failed = 0;
    for(const auto& test : tests){
        const string& name = test.first;
        const function<void()>& fun = test.second;
        try{
            fun();
            cout << "Passed " << name << '\n';
            passed++;
        }
        catch(const exception& e){
            cout << "Failed " << name << ": " << e.what() << '\n';
            failed++;
        }
        
    }
    cout<< "\nTest passed: " << passed << ", Tests failed: " << failed << endl;
    return failed ==0 ? 0 : 1;
}