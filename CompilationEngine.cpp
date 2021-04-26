#include <iostream>

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
        scanner.init(fin);

        string outfile = name.substr(0, name.find(".jack")) + ".xml";
        fout.open(outfile);

        // while(scanner.has_next()){
        //     Token token = scanner.next();
        //     fout << token.to_string() << endl;
        // }
        if(scanner.has_next()){
            Token token = scanner.next();
            if(token.value != "class"){
                cerr << "Error. First token in a jack file must be a class declaration." << endl;
                exit(-1);
            }
            compile_class();
        }

        fout.close();
        fin.close();
        indents = 0; //reset the indent counter after done with file
    }
}

//compiles a complete class
void CompilationEngine::compile_class(){
    fout << "<class>" << endl;
    indents++;
    fout << 
    if(scanner.has_next()){

    }
}