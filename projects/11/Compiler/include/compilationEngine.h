#ifndef COMPILATIONENGINE_H
#define COMPILATIONENGINE_H

#include "tokenizer.h"
#include "symbolTable.h"
#include "vmWriter.h"
#include "define.h"
#include <fstream>
#include <string>

class CompilationEngine {
public:
    CompilationEngine(const std::string& inputFilePath, const std::string& outputFilePath);
    void CompileClass();
    void CompileClassVarDec();
    void CompileSubroutineDec();
    void compileParameterList();
    void compileSubroutineBody();
    void compileVarDec();
    void compileStatements();
    void compileSubroutineCall();
    void compileSubroutineCall(const std::string& varname);
    void compileLet();
    void compileIf();
    void compileWhile();
    void compileDo();
    void compileReturn();
    void compileExpression();
    void compileTerm();
    void compileExpressionList(int& nArgs);
    ~CompilationEngine();

private:
    Tokenizer tokenizer;
    std::ofstream outputFile;
    SymbolTable symbolTable;
    VMWriter vmWriter;
    std::string className;
    std::string subroutineName;
    int labelCounter;
    void requireSymbol(char symbol);
    void requireKeyword(const std::string& keyword);
    void requireKeyword(std::initializer_list<Keyword> keywords);
    void requireTokenType(TokenType tokenType);
    void requireTokenType(std::initializer_list<TokenType> tokenTypes);
};

#endif