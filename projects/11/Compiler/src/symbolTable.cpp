#include "symbolTable.h"

SymbolTable::SymbolTable()
    : staticCount(0), fieldCount(0), argCount(0), varCount_(0), _subroutineType(Keyword::NULL_){}

void SymbolTable::startSubroutine(Keyword subroutineType) {
    subroutineScope.clear();
    this->_subroutineType = subroutineType;
    argCount = 0;
    varCount_ = 0;
}

void SymbolTable::define(const std::string& name, const std::string& type, Kind kind) {
    if (kind == Kind::STATIC) {
        classScope[name] = std::make_tuple(type, kind, staticCount++);
    } else if (kind == Kind::FIELD) {
        classScope[name] = std::make_tuple(type, kind, fieldCount++);
    } else if (kind == Kind::ARG) {
        subroutineScope[name] = std::make_tuple(type, kind, argCount++);
    } else if (kind == Kind::VAR) {
        subroutineScope[name] = std::make_tuple(type, kind, varCount_++);
    }
}

int SymbolTable::varCount(Kind kind) {
    if (kind == Kind::STATIC) {
        return staticCount;
    } else if (kind == Kind::FIELD) {
        return fieldCount;
    } else if (kind == Kind::ARG) {
        return argCount;
    } else if (kind == Kind::VAR) {
        return varCount_;
    }
    return 0;
}

Kind SymbolTable::kindOf(const std::string& name) {
    if (subroutineScope.find(name) != subroutineScope.end()) {
        return std::get<1>(subroutineScope[name]);
    } else if (classScope.find(name) != classScope.end()) {
        return std::get<1>(classScope[name]);
    }
    return Kind::NONE;
}

std::string SymbolTable::typeOf(const std::string& name) {
    if (subroutineScope.find(name) != subroutineScope.end()) {
        return std::get<0>(subroutineScope[name]);
    } else if (classScope.find(name) != classScope.end()) {
        return std::get<0>(classScope[name]);
    }
    return "";
}

int SymbolTable::indexOf(const std::string& name) {
    if (subroutineScope.find(name) != subroutineScope.end()) {
        return std::get<2>(subroutineScope[name]);
    } else if (classScope.find(name) != classScope.end()) {
        return std::get<2>(classScope[name]);
    }
    return -1;
}
