#include <iostream>
#include "Token.h"

//constructors
Token::Token(){}

Token::Token(token_type t, string val){
    type = t;
    value = val;
}

/*
 *return the XML representation of the token
 */
string Token::to_string(){
    string t;
    switch(type){
        case keyword:
            t = "keyword";
        case symbol:
            t = "symbol";
        case integer_constant:
            t = "integerConstant";
        case string_constant:
            t = "stringConstant";
        case identifier:
            t = "identifier";
    }

    return ("<" + t + ">" + " " + value + " " + "</" + t + ">");
}