#include <regex>
#include <iostream>
#include <algorithm>

#include "Scanner.h"

string remove_whitespace(string str){
    str.erase(remove(str.begin(), str.end(), ' '), str.end());
    return str;
}

/*
 * constructor. takes in a file stream and reads the file into a single string
 * while also filtering out single line comments
 */
Scanner::Scanner(ifstream &file){
    while(!file.eof()){
        string line;
        getline(file, line);
        if(line.find("//")){
            line = line.substr(0, line.find("//"));
        }
        contents += line;
    }
    contents = remove_whitespace(contents);
}

/*
 * gets the next token (but doesn't advance)
 * loop over regexes (starting with multiline comments)
 * if there is a match (besides a comment), return a token object 
 * that represents the token type and the value
 * if it is a comment, advance and restart the loop until a valid token is found
 */
Token Scanner::peek(){
    for(string reg: all_regexes){
        cout << "testing pattern: " << reg << endl;
        regex pattern(reg);
        smatch m;
        regex_search(contents, m, pattern, regex_constants::match_continuous);
        if(!m.empty()){
            if(reg == multiline_comment_pattern || reg == singleline_comment_pattern){
                size_t pos = contents.find(m.str()) + m.str().length();
                contents = contents.substr(pos); //cut out the comment
            }
            else if(reg == keyword_pattern){
                Token t(keyword, m.str());
                return t;
            }
            else if(reg == symbol_pattern){
                Token t(symbol, m.str());
                return t;
            }
            else if(reg == integer_constant_pattern){
                int i = stoi(m.str());
                if(i > 32767){
                    cerr << "Error. Integer constant exceeds maximum allowable value." << endl;
                    exit(-1);
                }
                Token t(integer_constant, m.str());
                return t;
            }
            else if(reg == string_constant_pattern){
                Token t(string_constant, m.str());
                return t;
            }
            else if(reg == identifier_pattern){
                Token t(identifier, m.str());
                return t;
            }
        }
    }
}

/*
 *gets the next token and advances
 */
Token Scanner::next(){
    Token token = peek(); //use peek to get next token
    //advance past the found token
    size_t pos = contents.find(token.value) + token.value.length();
    contents = contents.substr(pos); 
    return token;
}

//performs some regex searches to see if there are still token left 
bool Scanner::has_next(){
    for(string reg: token_regexes){
        regex pattern(reg);
        smatch m;
        regex_search(contents, m, pattern, regex_constants::match_continuous);
        if(!m.empty()){
            return true;
        }
    }
}


