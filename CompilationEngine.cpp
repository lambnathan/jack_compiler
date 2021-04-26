#include <iostream>

#include "CompilationEngine.h"
#include "Scanner.h"
#include "Token.h"

/*
 * helper function to repeat a given string n times
 */
string repeat(string s, int n){
    if(n == 0){
        return "";
    }
    string ret = "";
    for(int i = 0; i < n; i++){
        ret += s;
    }
    return ret;
}

CompilationEngine::CompilationEngine(vector<string> files){
    filenames = files;
}

void CompilationEngine::compile(){
    // go through each .jack file
    for(string name: filenames){
        cout << "compiling: " << name << endl;
        ifstream fin(name);
        scanner.init(fin);
        string outfile = name.substr(0, name.find(".jack")) + ".xml";
        fout.open(outfile);

        // while(scanner.has_next()){
        //     Token token = scanner.next();
        //     fout << token.to_string() << endl;
        // }
        if(scanner.has_next()){
            Token token = scanner.next();
            if(token.value != "class"){
                cerr << "Error. First token in a jack file must be a class declaration." << endl;
                cout << "actual was: " << token.value << endl;
                exit(-1);
            }
            compile_class();
        }

        fout.close();
        fin.close();
        indents = 0; //reset the indent counter after done with file
    }
}

/* compiles a complete class
 * starts with the class keyword, then calls classVarDec
 * after class var dec, subroutine decs should come next
 * this should end with a closing symbol
 */
void CompilationEngine::compile_class(){
    string ind = "";
    fout << "<class>" << endl;
    indents++;
    ind = repeat("\t", indents);
    fout << ind << "<keyword> class </keyword>" << endl; 
    if(scanner.has_next()){
        //next should be an identifier
        Token id = scanner.next();
        if(id.type != identifier){
            cerr << "Error. Expected an identifier after class." << endl;
            exit(-1);
        }
        fout << ind << "<identifier> " << id.value << " </identifier>" << endl;
        Token open_symbol = scanner.next();
        if(open_symbol.type != symbol){
            cerr << "Error. Expected a symbol after class name." << endl;
            exit(-1);
        }
        fout << ind << "<symbol> " << open_symbol.value << " </symbol>" << endl;
        Token next_token = scanner.peek();
        if(next_token.value == "static" || next_token.value == "field"){
            compile_classVarDec();
        }
        else{
            cerr << "Error. Expected \"static\" or \"field\" variable declarations." << endl;
            exit(-1);
        }

        //get subroutine info
        compile_subroutineDec();

        Token closing_symbol = scanner.next();
        if(closing_symbol.type != symbol){
            cerr << "Error. Expected a closing symbol." << endl;
            exit(-1);
        }
        fout << ind << "<symbol> " << closing_symbol.value << " </symbol>" << endl;
        indents--;
        fout << "</class>" << endl;
    }
    else{
        cerr << "Error. Expected an identifier after class." << endl;
        exit(-1);
    }
}

//compiles class variable declariations
void CompilationEngine::compile_classVarDec(){
    string ind = repeat("\t", indents);
    fout << ind << "<classVarDec>" << endl;
    indents++;
    ind = repeat("\t", indents);
    Token static_or_field = scanner.next();
    if(static_or_field.value == "static"){
        fout << ind << "<keyword> static </keyword>" << endl;
    }
    else if(static_or_field.value == "field"){
        fout << ind << "<keyword> field </keyword>" << endl;
    }
    else{
        cerr << "Error. Expected \"static\" or \"field\" variable declarations." << endl;
        exit(-1);
    }
    compile_type();

    //now must see at least one identifier for varname
    Token i = scanner.next();
    if(i.type != identifier){
        cerr << "Error. Expecting an identifier." << endl;
        exit(-1);
    }
    fout << ind << "<identifier> " << i.value << " </identifier>" << endl;

    //now loop, outputting comma symbols and additional identifiers until able to output semicolon
    while(scanner.peek().value != ";"){
        Token symbol_or_ident = scanner.next();
        if(symbol_or_ident.value == ","){
            fout << ind << "<symbol> , </symbol>" << endl;
        }
        else if(symbol_or_ident.type == identifier){
            fout << ind << "<identifier> " << symbol_or_ident.value << " </identifier>" << endl;
        }
    }
    Token semicolon = scanner.next();
    if(semicolon.value != ";"){
        cerr << "Error. Expected a semicolon afer class variable declarations." << endl;
        exit(-1);
    }
    fout << ind << "<symbol> ; </symbol>" << endl;
    indents--;
    ind = repeat("\t", indents);
    fout << ind << "</classVarDec>" << endl;
}

//handles different types
void CompilationEngine::compile_type(){
    string ind = repeat("\t", indents);
    Token typ = scanner.next();
    if(typ.value == "int" || typ.value == "char" || typ.value == "boolean" || typ.value == "void"){
        fout << ind << "<keyword> " << typ.value << " </keyword>" << endl;
    }
    else if(typ.type == identifier){
        fout << ind << "<identifier> " << typ.value << "< /identifier>" << endl;
    }
}

//compiles the declaration for a subroutine
void CompilationEngine::compile_subroutineDec(){
    string ind = repeat("\t", indents);
    fout << ind << "<subroutineDec>" << endl;
    indents++;
    ind = repeat("\t", indents);
    //first make sure keyword is either function, method, or constructor
    Token subtype = scanner.next();
    if(subtype.value == "function" || subtype.value == "method" || subtype.value == "constructor"){
        fout << ind << "<keyword> " << subtype.value << " </keyword>" << endl;
        //next token should be a type keyword
        compile_type();
        //next should be an identifier
        Token subname = scanner.next();
        if(subname.type != identifier){
            cerr << "Error. Expected an identifier after type." << endl;
            exit(-1);
        }
        fout << ind << "<identifier> " << subname.value << " </identifier>" << endl;
        Token open_symbol = scanner.next();
        if(open_symbol.type != symbol){
            cerr << "Error. Expected an open symbol after subroutine name." << endl;
            exit(-1);
        }
        fout << ind << "<symbol> " << open_symbol.value << " </symbol>" << endl;
        //now call code to compile parameter list (may not be any parameters)
        compile_parameterList();

        Token closing_symbol = scanner.next();
        if(closing_symbol.type != symbol){
            cerr << "Error. Expected a closing symbol after subroutine declaration." << endl;
            exit(-1);
        }
        fout << ind << "<symbol> " << closing_symbol.value << " </symbol>" << endl;
        //at end, call compile subroutine body function
        compile_subroutine();
    }
    else{
        cerr << "Error. Expected a keyword specifying the type of subroutine." << endl;
        exit(-1);
    }

    //after subroutinedec recursion is done, close tag
    indents--;
    ind = repeat("\t", indents);
    fout << ind << "</subroutineDec>" << endl;
}

//compiles a complete function, method, or constructor
void CompilationEngine::compile_subroutine(){

}

//compiles a (possibly empty) parameter list (not inluding the parenthesis)
void CompilationEngine::compile_parameterList(){

}
