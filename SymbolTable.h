#pragma once 

#include <string>
#include <vector>
#include <map>

#include "SymbolRecord.h"

using namespace std;

class SymbolTable{
    public:
        SymbolTable();

        SymbolRecord get(string name);
        void put(string name, string type, string segment, int offset);
        bool contains(string name);
        void clear();

        int num_fields = 0; //stores the number of fields (only for the global table)

        void print(); //for debugging  

    private:
        map<string, SymbolRecord> symbol_table;

};
