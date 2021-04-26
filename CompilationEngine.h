#pragma once

#include <fstream>
#include <vector>
#include <string>

using namespace std;

class CompilationEngine{
    public:
        CompilationEngine(vector<string> filenames);
        void compile();

    private:
        vector<string> filenames;
        int indents = 0;
        ofstream fout;
        Scanner scanner;

        void compile_class();
        void compile_classVarDec();
        void compile_subroutine();
        void compile_parameterList();
        void compile_varDec();
        void compile_statements();
        void compile_do();
        void compile_let();
        void compile_while();
        void compile_return();
        void compile_if();
        void compile_expression();
        void compile_term();
};