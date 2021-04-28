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
    string val = value;
    switch(type){
        case keyword:
            t = "keyword";
            break;
        case symbol:
            t = "symbol";
            if(value == "&") {val = "&amp;";}
            if(value == "<") {val = "&lt;";}
            if(value == ">") {val = "&gt;";}
            break;
        case integer_constant:
            t = "integerConstant";
            break;
        case string_constant:
            t = "stringConstant";
            val = value.substr(1, value.length() - 2); //to remove quotes
            break;
        case identifier:
            t = "identifier";
    }
    return ("<" + t + ">" + " " + val + " " + "</" + t + ">");
}