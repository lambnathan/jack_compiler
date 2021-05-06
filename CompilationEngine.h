#pragma once

#include <fstream>
#include <vector>
#include <string>

#include "Scanner.h"
#include "SymbolTable.h"

using namespace std;

class CompilationEngine{
    public:
        CompilationEngine(vector<string> files);
        void compile();

    private:
        vector<string> filenames;
        int indents = 0; //keeps track of how many indents currently
        string ind = ""; //actual string of indents
        string space_char = "  "; //string that is used to indent. either a series of spaces or a tab character
        ofstream fout;
        Scanner scanner;

        //things needed for actual code generation
        SymbolTable global_table; //holds statics and fields. gets created fresh for every class/.jack file
        SymbolTable local_table; //holds all local variables. gets created fresh for every function
        int class_offset = 0;
        int local_offset = 0;
        int arg_offset = 0;
        string current_class;
        string current_subroutine; 
        string function_type; //either function, method, or constructor
        int if_counter = 0; //appended to labels for if statements, making sure they are unique
        int while_counter = 0; //appended to labels for while statements, making sure they are unique

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
        int compile_expressionList(); //returns the number of expressions in the expression list
        void compile_term();
        void compile_type();
        void compile_subroutineCall();
};