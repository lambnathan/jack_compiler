

#include "SymbolTable.h"
#include "SymbolRecord.h"

SymbolTable::SymbolTable(){}

//returns the SymbolRecord associated with the given name
SymbolRecord SymbolTable::get(string name){
    return symbol_table[name];
}

//creates a SymbolRecord from the given arguments and inserts it into the tbale
void SymbolTable::put(string name, string type, string segment, int offset){
    SymbolRecord rec(type, segment, offset);
    symbol_table[name] = rec;
}

//checks if there is already a symboltable entry
bool SymbolTable::contains(string name){
    return symbol_table.count(name);
}

//clears the symboltable
void SymbolTable::clear(){
    symbol_table.clear();
}