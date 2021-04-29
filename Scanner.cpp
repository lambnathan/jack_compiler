#include <regex>
#include <iostream>

#include "Scanner.h"

string remove_leading_whitespace(string str){
    //make sure string is long enough and that first char is actually whitespace
    while(str.length() > 1 && (str[0] == ' ' || str[0] == '\t')){
        if(str.find_first_not_of(' ') == string::npos || str.find_first_not_of('\t') == string::npos){
            //string of all spaces or tabs
            return "";
        }
        if(str[0] == ' '){
            str = str.substr(str.find_first_not_of(' '));
        }
        else if(str[0] == '\t'){
            str = str.substr(str.find_first_not_of('\t'));
        }
    }
    return str;
}

string remove_trailing_whitespace(string str){
    if(str.length() > 1){
        str.erase(str.find_last_not_of(' ') + 1, string::npos);
    }
    return str;
}

/*
 * constructor.
 */
Scanner::Scanner(){}

/* initialize the scanner. happens once for every jack file. 
 * takes in a file stream and reads the file into a single string
 * while also filtering out single line comments
 */
void Scanner::init(ifstream &file){
    contents = "";
    while(!file.eof()){
        string line;
        getline(file, line);
        if(line.length() >= 2 && line[0] == '/' && line[1] == '/'){
            continue;
        }
        if(line.find("//")){
            line = line.substr(0, line.find("//"));
        }
        if(line[line.length() - 1] == '\r'){ //handle files written on Windows
            line = line.substr(0, line.length() - 1);
        }
        contents += line;
    }
    contents = remove_leading_whitespace(contents);
    //cout << "contents after init: " << contents << endl;
}

/*
 * gets the next token (but doesn't advance)
 * loop over regexes (starting with multiline comments)
 * if there is a match (besides a comment), return a token object 
 * that represents the token type and the value
 * if it is a comment, advance and restart the loop until a valid token is found
 */
Token Scanner::peek(){
    //cout << "current content: " << contents << endl;
    for(string reg: all_regexes){
        regex pattern(reg);
        smatch m;
        regex_search(contents, m, pattern, regex_constants::match_continuous);
        if(!m.empty()){
            if(reg == multiline_comment_pattern || reg == singleline_comment_pattern){
                size_t pos = contents.find(m.str()) + m.str().length();
                contents = contents.substr(pos); //cut out the comment
                contents = remove_leading_whitespace(contents);
                return peek();
            }
            else if(reg == keyword_pattern){
                //if its a keyword pattern, also have to remove trailing space
                string val = remove_trailing_whitespace(m.str());
                Token t(keyword, val);
                return t;
            }
            else if(reg == symbol_pattern){
                Token t(symbol, m.str());
                return t;
            }
            else if(reg == integer_constant_pattern){
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
    //cout << "contents: " << contents << endl;
    return Token(null, "null"); 
}

/*
 *special peek function, used only by CompilationEngine::compile_term()
 * peeks two tokens ahead instead of just one
 */
Token Scanner::peek_two(){
    string backup_contents = contents;
    Token not_needed = next();
    Token actual = next();
    contents = backup_contents; //restore contents back to their original
    return actual;
}

/*
 *gets the next token and advances
 *this function also does some simple checking (e.g int constants < 32767)
 */
Token Scanner::next(){
    Token token = peek(); //use peek to get next token
    if(token.type == integer_constant){
        int i = stoi(token.value);
        if(i > 32767){
            cerr << "Error. Integer constant exceeds maximum allowable value." << endl;
            exit(-1);
        }
    }
    if(token.type == null){
        cerr << "Error. Could not find a valid token type." << endl;
        exit(-1);
    }

    //advance past the found token
    size_t pos = contents.find(token.value) + token.value.length();
    contents = contents.substr(pos);
    contents = remove_leading_whitespace(contents); 
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


