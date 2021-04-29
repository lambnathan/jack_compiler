#pragma once

#include <string>

class SymbolRecord{
    public:
        SymbolRecord(string t, string s, int o){
            type = t;
            segment = s;
            offset = o;
        }
        std::string type;
        std::string segment;
        int offset;

};