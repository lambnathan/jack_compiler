#include "SymbolRecord.h"

SymbolRecord::SymbolRecord(){};

SymbolRecord::SymbolRecord(string t, string s, int o){
    type = t;
    segment = s;
    offset = o;
}