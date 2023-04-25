#include <string>
#include <iostream>
#include "lex.h"
#include "gram.h"
using namespace std;


int main() {
    string source_file = "../io/source.txt";
    string lex_result_file = "../io/lex_result.txt";
    string grammar_file = "../io/grammar.txt";
    Lex l;
    l.outputResult(source_file,lex_result_file);
    LR1 grammar(grammar_file);


}

