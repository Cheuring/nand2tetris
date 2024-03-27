#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <fstream>
#include "define.h"

class Tokenizer {
public:
    Tokenizer(const std::string& filename);
    ~Tokenizer();
    bool hasMoreTokens();
    void advance();
    TokenType tokenType();
    Keyword keyword();
    char symbol();
    std::string identifier();
    int intVal();
    std::string stringVal();
    size_t lineNumber();
private:
    std::ifstream ifs;
    std::string currentLine;
    std::string currentToken;
    size_t _lineNumber;
    void skipSpaces();
    void skipComments();
    void skipLineComments();
    void skipBlockComments();
};

#endif