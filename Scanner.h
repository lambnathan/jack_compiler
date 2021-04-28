#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "Token.h"

using namespace std;

class Scanner{
    public:
        Scanner();
        void init(ifstream &file);
        Token peek();
        Token peek_two(); //special peek function, used only by CompilationEngine::compile_term()
        Token next();
        bool has_next();


    private:
        //regular expressions
        string multiline_comment_pattern = R"(/\*.*?\*/)"; //nongreedy
        string singleline_comment_pattern = R"(//.*)";
        string keyword_pattern = "(class *|constructor *|function *|method *|field *|static *|var *|int *|char *|boolean *|void *|true *|false *|null *|this *|let *|do *|if *|else *|while *|return *)";
        string symbol_pattern = R"((\{|\}|\(|\)|\[|\]|\.|,|;|\+|-|\*|/|&|\||<|>|=|~))";
        string integer_constant_pattern = "([0-9]+)";
        string string_constant_pattern = R"(".*?")";
        string identifier_pattern = "[a-zA-Z_][a-zA-Z0-9_]*";
        vector<string> all_regexes = {multiline_comment_pattern, singleline_comment_pattern,keyword_pattern, 
                                  symbol_pattern, integer_constant_pattern, string_constant_pattern, identifier_pattern};
        vector<string> token_regexes = {keyword_pattern, symbol_pattern, integer_constant_pattern, 
                                        string_constant_pattern, identifier_pattern};
        string contents;
};