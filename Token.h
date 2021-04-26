#pragma once

#include <string>

using namespace std;

enum token_type {keyword, symbol, integer_constant, string_constant, identifier, null};

class Token{
    public:
        Token();
        Token(token_type t, string val);
        string to_string();

        token_type type;
        string value;
    private:
};