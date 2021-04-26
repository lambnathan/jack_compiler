#include <fstream>

#include "CompilationEngine.h"
#include "Scanner.h"
#include "Token.h"

CompilationEngine::CompilationEngine(vector<string> filenames){
    filenames = filenames;
}

void CompilationEngine::compile(){
    // go through each .jack file
    for(string name: filenames){
        ifstream fin(name);
        Scanner scanner(fin);

        string outfile = name.substr(0, name.find(".jack")) + ".xml";
        ofstream fout(outfile);

        while(scanner.has_next()){
            Token token = scanner.next();
            fout << token.to_string() << endl;
        }

        fout.close();
        fin.close();
    }
}