#pragma once

#include <fstream>
#include <vector>
#include <string>

#include "Scanner.h"

using namespace std;

class CompilationEngine{
    public:
        CompilationEngine(vector<string> files);
        void compile();

    private:
        vector<string> filenames;
        int indents = 0;
        ofstream fout;
        Scanner scanner;

        //helper functions to write opening and closing xml tags
        void write_open_tag(string val);
        void write_close_tag(string val);

        //recursive compile functions
        void compile_class();
        void compile_classVarDec();
        void compile_subroutine();
        void compile_subroutineDec();
        void compile_parameterList();
        void compile_varDec();
        void compile_statements();
        void compile_letStatement();
        void compile_ifStatement();
        void compile_whileStatement();
        void compile_doStatement();
        void compile_returnStatement();
        void compile_expression();
        void compile_expressionList();
        void compile_term();
        void compile_type();
        void compile_subroutineCall();
};