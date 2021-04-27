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
            Token token = scanner.peek();
            if(token.value != "class"){
                cerr << "Error. First token in a jack file must be a class declaration." << endl;
                cout << "actual was: " << token.value << endl;
                exit(-1);
            }
            compile_class();
        }
        else{
            cerr << "No tokens recieved." << endl;
            exit(-1);
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

    Token class_token = scanner.next();
    fout << ind << "<keyword> class </keyword>" << endl; 
    Token class_name = scanner.next();
    if(class_name.type != identifier){
        cerr << "Error. Expected a class name. (class)" << endl;
        exit(-1);
    }
    fout << ind << class_name.to_string() << endl;
    Token open_brace = scanner.next();
    if(open_brace.value != "{"){
        cerr << "Error. Expected an open brace after class name. (class)" << endl;
        exit(-1);
    }
    fout << ind << open_brace.to_string() << endl;
    while(scanner.peek().value != "}"){ //loop until end of class declaration
        Token what_dec = scanner.peek();
        if(what_dec.value == "static" || what_dec.value == "field"){ //
            compile_classVarDec();
        }
        else if(what_dec.value == "constructor" || what_dec.value == "method" || what_dec.value == "function"){
            compile_subroutineDec();
        }
    }
    Token closing_brace = scanner.next();
    fout << ind << closing_brace.to_string() << endl;
    indents--;
    fout << "</class>" << endl;
}

//compiles class variable declariations
void CompilationEngine::compile_classVarDec(){
    string ind = repeat("\t", indents);
    fout << ind << "<classVarDec>" << endl;
    indents++;
    ind = repeat("\t", indents);

    Token static_or_field = scanner.next();
    fout << ind << static_or_field.to_string() << endl;
    compile_type();

    //now must see at least one identifier for varname
    Token varname = scanner.next();
    if(varname.type != identifier){
        cerr << "Error. Expecting an identifier. (classVarDec)" << endl;
        exit(-1);
    }
    fout << ind << varname.to_string() << endl;

    //now loop, outputting comma symbols and additional identifiers until able to output semicolon
    while(scanner.peek().value != ";"){
        Token comma = scanner.next();
        if(comma.value != ","){
            cerr << "Error. Expected a comma for more variables. (classVarDec)" << endl;
            exit(-1);
        }
        fout << ind << comma.to_string() << endl;
        Token next_var_name = scanner.next();
        fout << ind << next_var_name.to_string() << endl;
    }
    Token semicolon = scanner.next();
    fout << ind << semicolon.to_string() << endl;

    indents--;
    ind = repeat("\t", indents);
    fout << ind << "</classVarDec>" << endl;
}

//handles different types
void CompilationEngine::compile_type(){
    string ind = repeat("\t", indents);
    Token typ = scanner.next();
    fout << ind << typ.to_string() << endl;
    // if(typ.value == "int" || typ.value == "char" || typ.value == "boolean" || typ.value == "void"){
    //     fout << ind << "<keyword> " << typ.value << " </keyword>" << endl;
    // }
    // else if(typ.type == identifier){
    //     fout << ind << "<identifier> " << typ.value << " </identifier>" << endl;
    // }
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
            cerr << "Error. Expected an identifier after type. (subroutineDec)" << endl;
            exit(-1);
        }
        fout << ind << subname.to_string() << endl;
        Token open_symbol = scanner.next();
        if(open_symbol.value != "("){
            cerr << "Error. Expected an open symbol after subroutine name. (subroutineDec)" << endl;
            exit(-1);
        }
        fout << ind << open_symbol.to_string() << endl;
        //now call code to compile parameter list (may not be any parameters)
        compile_parameterList();

        Token closing_symbol = scanner.next();
        if(closing_symbol.value != ")"){
            cerr << "Error. Expected a closing symbol after subroutine declaration. (subroutineDec)" << endl;
            exit(-1);
        }
        fout << ind << closing_symbol.to_string() << endl;
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

//compiles the body of a complete function, method, or constructor
void CompilationEngine::compile_subroutine(){
    string ind = repeat("\t", indents);
    fout << ind << "<subroutineBody>" << endl;
    indents++;
    ind = repeat("\t", indents);

    Token open_symbol = scanner.next();
    if(open_symbol.value != "{"){
        cerr << "Error. Opening curly brace expected for subroutine body. (subroutine)" << endl;
        exit(-1);
    }
    fout << ind << open_symbol.to_string() << endl;
    //now there should either be variable declartations, or statements
    //loop until the next closing curly brace is found
    while(scanner.peek().value != "}"){
        Token next_kwd = scanner.peek();
        if(next_kwd.value == "var"){
            compile_varDec();
        }
        else{
            compile_statements();
        }
    }

    Token closing_symbol = scanner.next();
    if(closing_symbol.value != "}"){
        cerr << "Error. Expected a closing curly brace after statements. (subroutine)" << endl;
        exit(-1);
    }
    fout << ind << closing_symbol.to_string() << endl;

    indents--;
    ind = repeat("\t", indents);
    fout << ind << "</subroutineBody>" << endl;
}

//compiles a (possibly empty) parameter list (not inluding the parenthesis)
void CompilationEngine::compile_parameterList(){
    string ind = repeat("\t", indents);
    fout << ind << "<parameterList>" << endl;
    indents++;
    ind = repeat("\t", indents);

    //to get all parameters, loop until we find closing paranthesis
    while(scanner.peek().value != ")"){
        //next should be a type. 
        compile_type();
        Token varname = scanner.next();
        if(varname.type != identifier){
            cerr << "Error. Expected a variable name after type." << endl;
            exit(-1);
        }
        fout << ind << "<identifier> " << varname.value << " </identifier>" << endl;
        if(scanner.peek().value == ","){//there is more, so consume the comma
            Token comma = scanner.next();
            fout << ind << "<symbol> , </symbol>" << endl; 
        }
    }

    indents--;
    ind = repeat("\t", indents);
    fout << ind << "</parameterList>" << endl;
}

//compiles a variable declaration
void CompilationEngine::compile_varDec(){
    string ind = repeat("\t", indents);
    fout << ind << "<varDec>" << endl;
    indents++;
    ind = repeat("\t", indents);

    //first token should be var
    Token var = scanner.next();
    if(var.value != "var"){
        cerr << "Error. Expected var keyword." << endl;
        exit(-1);
    }
    fout << ind << "<keyword> var </keyword>" << endl;
    //next comes the type of var
    compile_type();
    //next comes the varname
    Token varname = scanner.next();
    if(varname.type != identifier){
        cerr << "Error. Expected an identifier after var keyword." << endl;
        exit(-1);
    }
    fout << ind << "<identifier> " << varname.value << " </identifier>" << endl;
    //if there is not a semi colon next, there are more variables being declared
    while(scanner.peek().value != ";"){
        //should be a comma sperating vars
        Token comma = scanner.next();
        if(comma.value != ","){
            cerr << "Error. Expected a comma between variable declarations on same line." << endl;
            exit(-1);
        }
        fout << ind << "<symbol> , </symbol>" << endl;
        //now get next varname
        Token nextvar = scanner.next();
        if(nextvar.type != identifier){
            cerr << "Error. Expected a variable name." << endl;
            exit(-1);
        }
        fout << ind << "<identifier> " << nextvar.value << " </identifier>" << endl;
    }
    Token semicolon = scanner.next();
    if(semicolon.value != ";"){
        cerr << "Error. Expected a semicolon. (vardec)" << endl;
        exit(-1);
    }
    fout << ind << semicolon.to_string() << endl;

    indents--;
    ind = repeat("\t", indents);
    fout << ind << "</varDec>" << endl;
}

//compiles a sequence of statements, not including the enclosing curly braces
void CompilationEngine::compile_statements(){
    string ind = repeat("\t", indents);
    fout << ind << "<statements>" << endl;
    indents++;
    ind = repeat("\t", indents);

    //loop over all statements
    while(scanner.peek().value == "let" || scanner.peek().value == "if" || scanner.peek().value == "while" || scanner.peek().value == "do" || scanner.peek().value == "return"){
        Token statement_type = scanner.peek();
        if(statement_type.value == "let"){
            compile_letStatement();
        }
        else if(statement_type.value == "if"){
            compile_ifStatement();
        }
        else if(statement_type.value == "while"){
            compile_whileStatement();
        }
        else if(statement_type.value == "do"){
            compile_doStatement();
        }
        else if(statement_type.value == "return"){
            compile_returnStatement();
        }
        else{
            cerr << "Error. Unrecognized statement type. (statements)" << endl;
            exit(-1);
        }
    }

    indents--;
    ind = repeat("\t", indents);
    fout << ind << "</statements>" << endl;
}

//compiles a let statement
void CompilationEngine::compile_letStatement(){
    string ind = repeat("\t", indents);
    fout << ind << "<letStatement>" << endl;
    indents++;
    ind = repeat("\t", indents);

    //first token should be let
    Token let_token = scanner.next();
    fout << ind << let_token.to_string() << endl;
    //next should be a varname
    Token varname = scanner.next();
    if(varname.type != identifier){
        cerr << "Error. Expected a variable name (let statement)." << endl;
        exit(-1); 
    }
    fout << ind << varname.to_string() << endl;
    //what comes next is either an array expression or a normal expression
    if(scanner.peek().value == "["){
        //this is an array expression
        Token open_square = scanner.next();
        fout << ind << open_square.to_string() << endl;
        compile_expression();
        Token close_square = scanner.next();
        if(close_square.value != "]"){
            cerr << "Error. Expected a closing square bracket for array. (let statement)" << endl;
            exit(-1);
        }
        fout << ind << close_square.to_string() << endl;
    }
    //then check for the equal sign
    Token equ = scanner.next();
    if(equ.value != "="){
        cerr << "Error. Expected an equal sign for the expression. (let statement)" << endl;
        exit(-1);
    }
    fout << ind << equ.to_string() << endl;
    compile_expression();
    Token semicolon = scanner.next();
    if(semicolon.value != ";"){
        cerr << "Error. Expected semicolon after expression. (let statement)" << endl;
        exit(-1);
    }
    fout << ind << semicolon.to_string() << endl;

    indents--;
    ind = repeat("\t", indents);
    fout << ind << "</letStatement>" << endl;
}

//compiles an if statement
void CompilationEngine::compile_ifStatement(){
    string ind = repeat("\t", indents);
    fout << ind << "<ifStatement>" << endl;
    indents++;
    ind = repeat("\t", indents);
    //get if token
    Token iif = scanner.next();
    if(iif.value != "if"){
        cerr << "Error. Expected if keyword. (ifStatement)" << endl;
        exit(-1);
    }
    fout << ind << iif.to_string() << endl;
    Token open_par = scanner.next();
    if(open_par.value != "("){
        cerr << "Error. Expected opening parenthesis for if statement. (ifStatement)" << endl;
        exit(-1);
    }
    fout << ind << open_par.to_string() << endl;
    //now there should be an expression
    compile_expression();
    Token closing_par = scanner.next();
    if(closing_par.value != ")"){
        cerr << "Error. Expected a closing parenthesis for if statement. (ifStatement)" << endl;
        exit(-1);
    }
    fout << ind << closing_par.to_string() << endl;
    //now get brackets and statements
    Token open_braces = scanner.next();
    if(open_braces.value != "{"){
        cerr << "Error. Expected open curly braces. (ifStatement)" << endl;
        exit(-1);
    }
    fout << ind << open_braces.to_string() << endl;
    //statements are insdie curly braces
    compile_statements();
    Token closing_braces = scanner.next();
    if(closing_braces.value != "}"){
        cerr << "Error. Expected closing braces. (ifStatement)" << endl;
        exit(-1);
    }
    fout << ind << closing_braces.to_string() << endl;
    //now there may or may not be an else branch
    if(scanner.peek().value == "else"){
        Token elset = scanner.next();
        fout << ind << elset.to_string() << endl;
        open_braces = scanner.next();
        if(open_braces.value != "{"){
            cerr << "Error. Expected open curly braces. (ifStatement)" << endl;
            exit(-1);
        }
        fout << ind << open_braces.to_string() << endl;
        //statements are insdie curly braces
        compile_statements();
        closing_braces = scanner.next();
        if(closing_braces.value != "}"){
            cerr << "Error. Expected closing braces. (ifStatement)" << endl;
            exit(-1);
        }
        fout << ind << closing_braces.to_string() << endl;
    }

    indents--;
    ind = repeat("\t", indents);
    fout << ind << "</ifStatement>" << endl; 
}

//compiles a while statement
void CompilationEngine::compile_whileStatement(){
    string ind = repeat("\t", indents);
    fout << ind << "<whileStatement>" << endl;
    indents++;
    ind = repeat("\t", indents);

    Token whilet = scanner.next();
    fout << ind << whilet.to_string() << endl;
    Token open_par = scanner.next();
    if(open_par.value != "("){
        cerr << "Error. Expected opening parenthesis for while statement. (whileStatement)" << endl;
        exit(-1);
    }
    fout << ind << open_par.to_string() << endl;
    //now there should be an expression
    compile_expression();
    Token closing_par = scanner.next();
    if(closing_par.value != ")"){
        cerr << "Error. Expected a closing parenthesis for while statement. (whileStatement)" << endl;
        exit(-1);
    }
    fout << ind << closing_par.to_string() << endl;
    //now get brackets and statements
    Token open_braces = scanner.next();
    if(open_braces.value != "{"){
        cerr << "Error. Expected open curly braces. (whileStatement)" << endl;
        exit(-1);
    }
    fout << ind << open_braces.to_string() << endl;
    //statements are insdie curly braces
    compile_statements();
    Token closing_braces = scanner.next();
    if(closing_braces.value != "}"){
        cerr << "Error. Expected closing braces. (whileStatement)" << endl;
        exit(-1);
    }
    fout << ind << closing_braces.to_string() << endl;

    indents--;
    ind = repeat("\t", indents);
    fout << ind << "</whileStatement>" << endl;
}

//compiles a return statement
void CompilationEngine::compile_returnStatement(){
    string ind = repeat("\t", indents);
    fout << ind << "<returnStatement>" << endl;
    indents++;
    ind = repeat("\t", indents);

    Token returnt = scanner.next();
    fout << ind << returnt.to_string() << endl;
    if(scanner.peek().value != ";"){
        compile_expression();
    }
    Token semicolon = scanner.next();
    if(semicolon.value != ";"){
        cerr << "Error. Expected a semi colon on return statement (returnStatement)" << endl;
        exit(-1);
    }
    fout << ind << semicolon.to_string() << endl;

    indents--;
    ind = repeat("\t", indents);
    fout << ind << "</returnStatement>" << endl;
}

//compiles a do statement
void CompilationEngine::compile_doStatement(){
    string ind = repeat("\t", indents);
    fout << ind << "<doStatement>" << endl;
    indents++;
    ind = repeat("\t", indents);

    Token dot = scanner.next();
    fout << ind << dot.to_string() << endl;
    if(scanner.peek().value == ";"){
        cerr << "Error. Expected a subroutine call after do statement. (doStatement)" << endl;
        exit(-1);
    }
    compile_subroutineCall();
    Token semicolon = scanner.next();
    if(semicolon.value != ";"){
        cerr << "Error. Expected a semi colon on do statement (doStatement)" << endl;
        exit(-1);
    }
    fout << ind << semicolon.to_string() << endl;

    indents--;
    ind = repeat("\t", indents);
    fout << ind << "</doStatement>" << endl;
}



//compiles a (possibly empty) expression list
void CompilationEngine::compile_expressionList(){
    string ind = repeat("\t", indents);
    fout << ind << "<expressionList>" << endl;
    indents++;
    ind = repeat("\t", indents);

    if(scanner.peek().value != ")"){ //expressionlist is not empty
        compile_expression();
        while(scanner.peek().value == ","){
            Token comma = scanner.next();
            fout << ind << comma.to_string() << endl;
            compile_expression();
        }
    }

    indents--;
    ind = repeat("\t", indents);
    fout << ind << "</expressionList>" << endl;
}


//compiles an expression
void CompilationEngine::compile_expression(){
    string ind = repeat("\t", indents);
    fout << ind << "<expression>" << endl;
    indents++;
    ind = repeat("\t", indents);

    //expressions are composed of terms.
    compile_term();
    string ops = "+-/*=><&|"; //string containing the possible binary operators
    //while there is still a binary op, the expression continues
    while(ops.find(scanner.peek().value) != string::npos){
        Token bop = scanner.next();
        fout << ind << bop.to_string() << endl;
        compile_term();
    }

    indents--;
    ind = repeat("\t", indents);
    fout << ind << "</expression>" << endl;
}

/*
 * compiles a term
 */
void CompilationEngine::compile_term(){
    string ind = repeat("\t", indents);
    fout << ind << "<term>" << endl;
    indents++;
    ind = repeat("\t", indents);

    string unaryops = "~-"; //string holding the possible unary operators

    Token term = scanner.peek();
    if(term.type == integer_constant || term.type == string_constant || term.type == keyword){
        term = scanner.next();
        fout << ind << term.to_string() << endl;
    }
    else if(unaryops.find(term.value) != string::npos){
        term = scanner.next();
        fout << ind << term.to_string() << endl;
        compile_term();
    }
    else if(term.type == identifier){ //could either be array indexing, a subroutine call (different forms), or just an identifier
        if(scanner.peek_two().value == "["){
            //array indexing situation
            fout << ind << term.to_string() << endl;
            Token open_symbol = scanner.next();
            open_symbol = scanner.next(); //need to consume two
            fout << ind << open_symbol.to_string() << endl;
            //now parse an expression
            compile_expression();
            Token close_symbol = scanner.next();
            if(close_symbol.value != "]"){
                cerr << "Error. Expected a closing square bracket. (term)" << endl;
                exit(-1);
            }
            fout << ind << close_symbol.to_string() << endl;
        }
        else if(scanner.peek_two().value == "(" || scanner.peek_two().value == "."){
            //subroutine call 
            compile_subroutineCall();
        }
        else{
            //just a normal identifier
            term = scanner.next();
            fout << ind << term.to_string() << endl;
        }
    }
    else if(term.type == symbol && term.value == "("){ //is an expression
        Token open_par = scanner.next();
        fout << ind << open_par.to_string() << endl;
        compile_expression();
        Token closing_par = scanner.next();
        if(closing_par.value != ")"){
            cerr << "Error. Expected a closing parenthesis. (term)" << endl;
            exit(-1);
        }
        fout << ind << closing_par.to_string() << endl;
    }

    indents--;
    ind = repeat("\t", indents);
    fout << ind << "</term>" << endl;
}

//compiles a subroutine call
//no actual xml tags for this
void CompilationEngine::compile_subroutineCall(){
    string ind = repeat("\t", indents);

    Token name = scanner.next();
    if(name.type != identifier){
        cerr << "Error. Expected an identifier for the subroutine. (subroutine_call)" << endl;
        exit(-1);
    }
    fout << ind << name.to_string() << endl;
    if(scanner.peek().value == "("){
        Token open_symbol = scanner.next();
        fout << ind << open_symbol.to_string() << endl;
        compile_expressionList();
        Token closing_symbol = scanner.next();
        if(closing_symbol.value != ")"){
            cerr << "Error. Expected a closing parenthesis. (subroutine_call)" << endl;
            exit(-1);
        }
        fout << ind << closing_symbol.to_string() << endl;
    }
    else if(scanner.peek().value == "."){
        Token dot = scanner.next();
        fout << ind << dot.to_string() << endl;
        name = scanner.next();
        if(name.type != identifier){
            cerr << "Error. Expected an identifier for subroutine name. (subroutine_call)" << endl;
            exit(-1);
        }
        fout << ind << name.to_string() << endl;
        Token open_symbol = scanner.next();
        fout << ind << open_symbol.to_string() << endl;
        compile_expressionList();
        Token closing_symbol = scanner.next();
        if(closing_symbol.value != ")"){
            cerr << "Error. Expected a closing parenthesis. (subroutine_call)" << endl;
            exit(-1);
        }
        fout << ind << closing_symbol.to_string() << endl;
    }
}