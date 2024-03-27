#include "tokenizer.h"
#include <sstream>
#include <algorithm>

Tokenizer::Tokenizer(const std::string& filename) : ifs(filename), _lineNumber(0) {
    if (!ifs) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    advance();
}

Tokenizer::~Tokenizer() {
    ifs.close();
}

bool Tokenizer::hasMoreTokens() {
    return ifs.good();
}

void Tokenizer::advance() {
    currentToken.clear();
    skipSpaces();
    while(currentLine.empty() && hasMoreTokens()) {
        std::getline(ifs, currentLine);
        _lineNumber++;
        skipComments();
        skipSpaces();
    }

    std::istringstream iss(currentLine);
    iss >> currentToken;
    std::string symbols =".,()[];+-*/&|<>=~\"{}";
    size_t pos = currentToken.find_first_of(symbols);
    if (pos != std::string::npos) {
        if(currentToken[pos]=='"'){
            size_t pos1=currentLine.find_first_of('"');
            size_t pos2=currentLine.find('"',pos1+1);
            currentToken = currentLine.substr(pos1,pos2-pos1+1);
        }else{
            std::string temp;
            temp = currentToken.substr(0, pos);
            currentToken = temp.empty() ? currentToken.substr(0,1) : temp;
        }
    }
    currentLine = currentLine.substr(currentLine.find(currentToken) + currentToken.size());
}

TokenType Tokenizer::tokenType() {
    if(currentToken=="class"||currentToken=="constructor"||currentToken=="function"||currentToken=="method"||
        currentToken=="field"||currentToken=="static"||currentToken=="var"||currentToken=="int"||
        currentToken=="char"||currentToken=="boolean"||currentToken=="void"||currentToken=="true"||
        currentToken=="false"||currentToken=="null"||currentToken=="this"||currentToken=="let"||
        currentToken=="do"||currentToken=="if"||currentToken=="else"||currentToken=="while"||
        currentToken=="return"){
        return TokenType::KEYWORD;
    }else if(currentToken=="{"||currentToken=="}"||currentToken=="("||currentToken==")"||
                currentToken=="["||currentToken=="]"||currentToken=="."||currentToken==","||
                currentToken==";"||currentToken=="+"||currentToken=="-"||currentToken=="*"||
                currentToken=="/"||currentToken=="&"||currentToken=="|"||currentToken=="<"||
                currentToken==">"||currentToken=="="||currentToken=="~"){
        return TokenType::SYMBOL;
    }else if(currentToken[0]=='"'&&currentToken[currentToken.length()-1]=='"'){
        return TokenType::STRING_CONST;
    }else if(currentToken[0]>='0'&&currentToken[0]<='9'){
        return TokenType::INT_CONST;
    }else{
        return TokenType::IDENTIFIER;
    }
}

Keyword Tokenizer:: keyword() {
    if(currentToken=="class"){
        return Keyword::CLASS;
    }else if(currentToken=="constructor"){
        return Keyword::CONSTRUCTOR;
    }else if(currentToken=="function"){
        return Keyword::FUNCTION;
    }else if(currentToken=="method"){
        return Keyword::METHOD;
    }else if(currentToken=="field"){
        return Keyword::FIELD;
    }else if(currentToken=="static"){
        return Keyword::STATIC;
    }else if(currentToken=="var"){
        return Keyword::VAR;
    }else if(currentToken=="int"){
        return Keyword::INT;
    }else if(currentToken=="char"){
        return Keyword::CHAR;
    }else if(currentToken=="boolean"){
        return Keyword::BOOLEAN;
    }else if(currentToken=="void"){
        return Keyword::VOID;
    }else if(currentToken=="true"){
        return Keyword::TRUE;
    }else if(currentToken=="false"){
        return Keyword::FALSE;
    }else if(currentToken=="null"){
        return Keyword::NULL_;
    }else if(currentToken=="this"){
        return Keyword::THIS;
    }else if(currentToken=="let"){
        return Keyword::LET;
    }else if(currentToken=="do"){
        return Keyword::DO;
    }else if(currentToken=="if"){
        return Keyword::IF;
    }else if(currentToken=="else"){
        return Keyword::ELSE;
    }else if(currentToken=="while"){
        return Keyword::WHILE;
    }
    return Keyword::RETURN;
}

char Tokenizer::symbol() {
    return currentToken[0];
}

std::string Tokenizer::identifier() {
    return currentToken;
}

int Tokenizer::intVal() {
    return std::stoi(currentToken);
}

std::string Tokenizer::stringVal() {
    return currentToken.substr(1, currentToken.size() - 2);
}

size_t Tokenizer::lineNumber() {
    return _lineNumber;
}

void Tokenizer::skipSpaces() {
    currentLine.erase(currentLine.begin(),find_if(currentLine.begin(),currentLine.end(),[](unsigned char ch){
        return !std::isspace(ch);
    }));
}

void Tokenizer::skipComments() {
    skipLineComments();
    skipBlockComments();
}

void Tokenizer::skipLineComments() {
    std::string::size_type pos;
    if((pos=currentLine.find("//"))!=std::string::npos){
        currentLine = currentLine.substr(0,pos);
    }
}

void Tokenizer::skipBlockComments() {
    std::string::size_type pos1,pos2;
    while((pos1=currentLine.find("/*"))!=std::string::npos){
        if((pos2=currentLine.find("*/"))!=std::string::npos){
            currentLine = currentLine.substr(0,pos1)+currentLine.substr(pos2+2);
        }else{
            std::string temp;
            do{
                std::getline(ifs,temp);
                _lineNumber++;
            }while((pos2=temp.find("*/"))==std::string::npos);
            currentLine = currentLine.substr(0,pos1)+temp.substr(pos2+2);
        }
    }
}