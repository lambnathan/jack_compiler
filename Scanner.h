#pragma once

#include <fstream>
#include <string>

#include "Token.h"

class Scanner{
    public:
        Scanner(std::ifstream &file);
        Token peek();
        Token next();


    private:
        //regular expressions
        std::string keyword_pattern = "(class|constructor|function|method|field|static|var|int|char|boolean|void|true|false|null|this|let|do|if|else|while|return)";
        std::string symbol_pattern = "(\{|\}|\(|\)|\[|\]|\.|,|;|\+|-|\*|/|&|\||<|>|=|~)";
        std::string integer_constant_pattern = "([1-9][0-9]*)";
        std::string string_constant_pattern = "\".*\"";
        std::string identifier_pattern = "[a-zA-Z_][a-zA-Z0-9_]*";
    

};