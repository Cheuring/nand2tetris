#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>
#include <unordered_map>
#include "define.h"

class SymbolTable {
public:
    SymbolTable();
    void startSubroutine(Keyword subroutineType);
    void define(const std::string& name, const std::string& type, Kind kind);
    int varCount(Kind kind);
    Kind kindOf(const std::string& name);
    std::string typeOf(const std::string& name);
    int indexOf(const std::string& name);
    Keyword subroutineType() { return _subroutineType; }
private:
    std::unordered_map<std::string, std::tuple<std::string, Kind, int>> classScope;
    std::unordered_map<std::string, std::tuple<std::string, Kind, int>> subroutineScope;
    int staticCount;
    int fieldCount;
    int argCount;
    int varCount_;
    Keyword _subroutineType;
};

#endif