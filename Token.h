#pragma once

#include <string>

enum token_type{keyword, symbol, integer_constant, string_constant, indentifier};

class Token{
    public:
        Token();
        token_type type;
        std::string value;
    private:
};