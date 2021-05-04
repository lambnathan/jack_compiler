#pragma once

#include <string>

using namespace std;

class SymbolRecord{
    public:
        SymbolRecord();
        SymbolRecord(string t, string s, int o);

        string type;
        string segment;
        int offset;

};