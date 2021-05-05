#include <iostream>

#include "SymbolTable.h"

SymbolTable::SymbolTable(){}

//returns the SymbolRecord associated with the given name
SymbolRecord SymbolTable::get(string name){
    return symbol_table[name];
}

//creates a SymbolRecord from the given arguments and inserts it into the tbale
void SymbolTable::put(string name, string type, string segment, int offset){
    SymbolRecord rec(type, segment, offset);
    symbol_table[name] = rec;
    if(segment == "this"){ //check if it is a field
        num_fields++;
    }
}

//checks if there is already a symboltable entry
bool SymbolTable::contains(string name){
    return symbol_table.count(name);
}

//clears the symboltable
void SymbolTable::clear(){
    symbol_table.clear();
}

//function for priting the symbol table, for debugging purposes
void SymbolTable::print(){
    cout << "Name\t Type\t Segment\t Offset" << endl;
    for(auto pair: symbol_table){
        cout << pair.first << "\t|" << pair.second.type << "\t|" << pair.second.segment << "\t|" << pair.second.offset << endl;
    } 
}