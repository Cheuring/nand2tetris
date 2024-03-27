#include "compilationEngine.h"
#include "define.h"
#include <algorithm>

CompilationEngine::CompilationEngine(const std::string& inputFilePath, const std::string& outputFilePath)
    : tokenizer(inputFilePath), vmWriter(outputFilePath), className(""), subroutineName(""), labelCounter(0) {}

void CompilationEngine::CompileClass() {
    requireKeyword("class");
    className = tokenizer.identifier();
    requireTokenType(TokenType::IDENTIFIER);
    requireSymbol('{');
    while (tokenizer.tokenType() == TokenType::KEYWORD && (tokenizer.keyword() == Keyword::STATIC || tokenizer.keyword() == Keyword::FIELD)) {
        CompileClassVarDec();
    }
    while (tokenizer.tokenType() == TokenType::KEYWORD && (tokenizer.keyword() == Keyword::CONSTRUCTOR || tokenizer.keyword() == Keyword::FUNCTION || tokenizer.keyword() == Keyword::METHOD)) {
        CompileSubroutineDec();
    }
    requireSymbol('}');
}

void CompilationEngine::CompileClassVarDec() {
    Kind kind = tokenizer.keyword() == Keyword::STATIC ? Kind::STATIC : Kind::FIELD;
    tokenizer.advance();
    std::string type = tokenizer.identifier();
    requireTokenType({TokenType::IDENTIFIER, TokenType::KEYWORD});
    symbolTable.define(tokenizer.identifier(), type, kind);
    requireTokenType(TokenType::IDENTIFIER);
    while (tokenizer.symbol() == ',') {
        tokenizer.advance();
        symbolTable.define(tokenizer.identifier(), type, kind);
        requireTokenType(TokenType::IDENTIFIER);
    }
    requireSymbol(';');
}

void CompilationEngine::CompileSubroutineDec() {
    symbolTable.startSubroutine(tokenizer.keyword());
    if (tokenizer.keyword() == Keyword::METHOD) {
        symbolTable.define("this", className, Kind::ARG);
    }
    requireKeyword({Keyword::CONSTRUCTOR, Keyword::FUNCTION, Keyword::METHOD});
    requireTokenType({TokenType::IDENTIFIER, TokenType::KEYWORD});
    subroutineName = tokenizer.identifier();
    requireTokenType(TokenType::IDENTIFIER);
    requireSymbol('(');
    compileParameterList();
    requireSymbol(')');
    requireSymbol('{');
    compileSubroutineBody();
}

void CompilationEngine::compileParameterList() {
    if (tokenizer.tokenType() == TokenType::SYMBOL && tokenizer.symbol() == ')') {
        return;
    }
    std::string type = tokenizer.identifier();
    requireTokenType({TokenType::IDENTIFIER, TokenType::KEYWORD});
    symbolTable.define(tokenizer.identifier(), type, Kind::ARG);
    requireTokenType(TokenType::IDENTIFIER);
    while (tokenizer.symbol() == ',') {
        tokenizer.advance();
        type = tokenizer.identifier();
        requireTokenType({TokenType::IDENTIFIER, TokenType::KEYWORD});
        symbolTable.define(tokenizer.identifier(), type, Kind::ARG);
        requireTokenType(TokenType::IDENTIFIER);
    }
}

void CompilationEngine::compileSubroutineBody() {
    while (tokenizer.tokenType() == TokenType::KEYWORD && tokenizer.keyword() == Keyword::VAR) {
        compileVarDec();
    }
    vmWriter.writeFunction(className + "." + subroutineName, symbolTable.varCount(Kind::VAR));
    if (symbolTable.subroutineType() == Keyword::CONSTRUCTOR) {
        vmWriter.writePush(Segment::CONST, symbolTable.varCount(Kind::FIELD));
        vmWriter.writeCall("Memory.alloc", 1);
        vmWriter.writePop(Segment::POINTER, 0);
    } else if (symbolTable.subroutineType() == Keyword::METHOD){
        vmWriter.writePush(Segment::ARG, 0);
        vmWriter.writePop(Segment::POINTER, 0);
    }
    compileStatements();
    requireSymbol('}');
}

void CompilationEngine::compileVarDec() {
    tokenizer.advance();
    std::string type = tokenizer.identifier();
    requireTokenType({TokenType::IDENTIFIER, TokenType::KEYWORD});
    symbolTable.define(tokenizer.identifier(), type, Kind::VAR);
    requireTokenType(TokenType::IDENTIFIER);
    while (tokenizer.symbol() == ',') {
        tokenizer.advance();
        symbolTable.define(tokenizer.identifier(), type, Kind::VAR);
        requireTokenType(TokenType::IDENTIFIER);
    }
    requireSymbol(';');
}

void CompilationEngine::compileStatements() {
    while (tokenizer.tokenType() == TokenType::KEYWORD && (tokenizer.keyword() == Keyword::LET || tokenizer.keyword() == Keyword::IF || tokenizer.keyword() == Keyword::WHILE || tokenizer.keyword() == Keyword::DO || tokenizer.keyword() == Keyword::RETURN)) {
        if (tokenizer.keyword() == Keyword::LET) {
            compileLet();
        } else if (tokenizer.keyword() == Keyword::IF) {
            compileIf();
        } else if (tokenizer.keyword() == Keyword::WHILE) {
            compileWhile();
        } else if (tokenizer.keyword() == Keyword::DO) {
            compileDo();
        } else if (tokenizer.keyword() == Keyword::RETURN) {
            compileReturn();
        }
    }
}

void CompilationEngine::compileSubroutineCall() {
    std::string subroutineName = tokenizer.identifier();
    requireTokenType(TokenType::IDENTIFIER);
    int nArgs = 0;
    if (tokenizer.symbol() == '(') {
        vmWriter.writePush(Segment::POINTER, 0);
        requireSymbol('(');
        compileExpressionList(nArgs);
        requireSymbol(')');
        vmWriter.writeCall(className + "." + subroutineName, nArgs + 1);
    } else if (tokenizer.symbol() == '.') {
        requireSymbol('.');
        std::string varName = subroutineName;
        subroutineName = tokenizer.identifier();
        requireTokenType(TokenType::IDENTIFIER);
        if (symbolTable.kindOf(varName) != Kind::NONE) {
            vmWriter.writePush(symbolTable.kindOf(varName), symbolTable.indexOf(varName));
            varName = symbolTable.typeOf(varName);
            nArgs = 1;
        }
        requireSymbol('(');
        compileExpressionList(nArgs);
        requireSymbol(')');
        vmWriter.writeCall(varName + "." + subroutineName, nArgs);
    }

}

void CompilationEngine::compileSubroutineCall(const std::string& varName) {
    requireSymbol('.');
    int nArgs = 0;
    std::string varName_=varName;
    std::string subroutineName = tokenizer.identifier();
    requireTokenType(TokenType::IDENTIFIER);
    if (symbolTable.kindOf(varName_) != Kind::NONE) {
        vmWriter.writePush(symbolTable.kindOf(varName_), symbolTable.indexOf(varName_));
        varName_ = symbolTable.typeOf(varName_);
        nArgs = 1;
    }
    requireSymbol('(');
    compileExpressionList(nArgs);
    requireSymbol(')');
    vmWriter.writeCall(varName_ + "." + subroutineName, nArgs);
}

void CompilationEngine::compileLet() {
    requireKeyword("let");
    std::string varName = tokenizer.identifier();
    requireTokenType(TokenType::IDENTIFIER);
    if (tokenizer.symbol() == '[') {
        vmWriter.writePush(symbolTable.kindOf(varName), symbolTable.indexOf(varName));
        requireSymbol('[');
        compileExpression();
        requireSymbol(']');
        vmWriter.writeArithmetic(Command::ADD);
        requireSymbol('=');
        compileExpression();
        requireSymbol(';');
        vmWriter.writePop(Segment::TEMP, 0);
        vmWriter.writePop(Segment::POINTER, 1);
        vmWriter.writePush(Segment::TEMP, 0);
        vmWriter.writePop(Segment::THAT, 0);
    } else {
        requireSymbol('=');
        compileExpression();
        requireSymbol(';');
        vmWriter.writePop(symbolTable.kindOf(varName), symbolTable.indexOf(varName));
    }
}

void CompilationEngine::compileIf() {
    requireKeyword("if");
    requireSymbol('(');
    compileExpression();
    requireSymbol(')');
    requireSymbol('{');
    int labelIndex = labelCounter++;
    vmWriter.writeIf("IF_TRUE" + std::to_string(labelIndex));
    vmWriter.writeGoto("IF_FALSE" + std::to_string(labelIndex));
    vmWriter.writeLabel("IF_TRUE" + std::to_string(labelIndex));
    compileStatements();
    requireSymbol('}');
    if (tokenizer.tokenType() == TokenType::KEYWORD && tokenizer.keyword() == Keyword::ELSE) {
        vmWriter.writeGoto("IF_END" + std::to_string(labelIndex));
        vmWriter.writeLabel("IF_FALSE" + std::to_string(labelIndex));
        requireKeyword("else");
        requireSymbol('{');
        compileStatements();
        requireSymbol('}');
        vmWriter.writeLabel("IF_END" + std::to_string(labelIndex));
    } else {
        vmWriter.writeLabel("IF_FALSE" + std::to_string(labelIndex));
    }
}

void CompilationEngine::compileWhile() {
    requireKeyword("while");
    int labelIndex = labelCounter++;
    vmWriter.writeLabel("WHILE_EXP" + std::to_string(labelIndex));
    requireSymbol('(');
    compileExpression();
    requireSymbol(')');
    vmWriter.writeArithmetic(Command::NOT);
    vmWriter.writeIf("WHILE_END" + std::to_string(labelIndex));
    requireSymbol('{');
    compileStatements();
    requireSymbol('}');
    vmWriter.writeGoto("WHILE_EXP" + std::to_string(labelIndex));
    vmWriter.writeLabel("WHILE_END" + std::to_string(labelIndex));
}

void CompilationEngine::compileDo() {
    requireKeyword("do");
    compileSubroutineCall();
    requireSymbol(';');
    vmWriter.writePop(Segment::TEMP, 0);
}

void CompilationEngine::compileReturn() {
    requireKeyword("return");
    if (tokenizer.tokenType() != TokenType::SYMBOL || tokenizer.symbol() != ';') {
        compileExpression();
    } else {
        vmWriter.writePush(Segment::CONST, 0);
    }
    requireSymbol(';');
    vmWriter.writeReturn();
}

void CompilationEngine::compileExpression() {
    compileTerm();
    while (tokenizer.tokenType() == TokenType::SYMBOL && (tokenizer.symbol() == '+' || tokenizer.symbol() == '-' || tokenizer.symbol() == '*' || tokenizer.symbol() == '/' || tokenizer.symbol() == '&' || tokenizer.symbol() == '|' || tokenizer.symbol() == '<' || tokenizer.symbol() == '>' || tokenizer.symbol() == '=')) {
        char op = tokenizer.symbol();
        tokenizer.advance();
        compileTerm();
        switch (op) {
            case '+':
                vmWriter.writeArithmetic(Command::ADD);
                break;
            case '-':
                vmWriter.writeArithmetic(Command::SUB);
                break;
            case '*':
                vmWriter.writeCall("Math.multiply", 2);
                break;
            case '/':
                vmWriter.writeCall("Math.divide", 2);
                break;
            case '&':
                vmWriter.writeArithmetic(Command::AND);
                break;
            case '|':
                vmWriter.writeArithmetic(Command::OR);
                break;
            case '<':
                vmWriter.writeArithmetic(Command::LT);
                break;
            case '>':
                vmWriter.writeArithmetic(Command::GT);
                break;
            case '=':
                vmWriter.writeArithmetic(Command::EQ);
                break;
        }
    }
}

void CompilationEngine::compileTerm() {
    if (tokenizer.tokenType() == TokenType::INT_CONST) {
        vmWriter.writePush(Segment::CONST, tokenizer.intVal());
        tokenizer.advance();
    } else if (tokenizer.tokenType() == TokenType::STRING_CONST) {
        std::string str = tokenizer.stringVal();
        vmWriter.writePush(Segment::CONST, str.length());
        vmWriter.writeCall("String.new", 1);
        for (char c : str) {
            vmWriter.writePush(Segment::CONST, c);
            vmWriter.writeCall("String.appendChar", 2);
        }
        tokenizer.advance();
    } else if (tokenizer.tokenType() == TokenType::KEYWORD && (tokenizer.keyword() == Keyword::TRUE || tokenizer.keyword() == Keyword::FALSE || tokenizer.keyword() == Keyword::NULL_ || tokenizer.keyword() == Keyword::THIS)) {
        if (tokenizer.keyword() == Keyword::TRUE) {
            vmWriter.writePush(Segment::CONST, 0);
            vmWriter.writeArithmetic(Command::NOT);
        } else if (tokenizer.keyword() == Keyword::FALSE || tokenizer.keyword() == Keyword::NULL_) {
            vmWriter.writePush(Segment::CONST, 0);
        } else if (tokenizer.keyword() == Keyword::THIS) {
            vmWriter.writePush(Segment::POINTER, 0);
        }
        tokenizer.advance();
    } else if (tokenizer.tokenType() == TokenType::SYMBOL && tokenizer.symbol() == '(') {
        requireSymbol('(');
        compileExpression();
        requireSymbol(')');
    } else if (tokenizer.tokenType() == TokenType::SYMBOL && (tokenizer.symbol() == '-' || tokenizer.symbol() == '~')) {
        char op = tokenizer.symbol();
        tokenizer.advance();
        compileTerm();
        switch (op) {
            case '-':
                vmWriter.writeArithmetic(Command::NEG);
                break;
            case '~':
                vmWriter.writeArithmetic(Command::NOT);
                break;
        }
    } else if (tokenizer.tokenType() == TokenType::IDENTIFIER) {
        std::string varName = tokenizer.identifier();
        requireTokenType(TokenType::IDENTIFIER);
        if (tokenizer.symbol() == '[') {
            vmWriter.writePush(symbolTable.kindOf(varName), symbolTable.indexOf(varName));
            requireSymbol('[');
            compileExpression();
            requireSymbol(']');
            vmWriter.writeArithmetic(Command::ADD);
            vmWriter.writePop(Segment::POINTER, 1);
            vmWriter.writePush(Segment::THAT, 0);
        } else if (tokenizer.symbol() == '(' || tokenizer.symbol() == '.') {
            compileSubroutineCall(varName);
        } else {
            vmWriter.writePush(symbolTable.kindOf(varName), symbolTable.indexOf(varName));
        }
    }
}

void CompilationEngine::compileExpressionList(int& nArgs) {
    if (tokenizer.tokenType() == TokenType::SYMBOL && tokenizer.symbol() == ')') {
        return;
    }
    compileExpression();
    nArgs++;
    while (tokenizer.symbol() == ',') {
        tokenizer.advance();
        compileExpression();
        nArgs++;
    }
}

void CompilationEngine::requireSymbol(char symbol) {
    if (tokenizer.tokenType() != TokenType::SYMBOL || tokenizer.symbol() != symbol) {
        throw std::runtime_error("Symbol error at line " + std::to_string(tokenizer.lineNumber()));
    }
    tokenizer.advance();
}

void CompilationEngine::requireKeyword(const std::string& keyword) {
    if (tokenizer.identifier() != keyword) {
        throw std::runtime_error("Keyword error at line " + std::to_string(tokenizer.lineNumber()));
    }
    tokenizer.advance();
}

void CompilationEngine::requireKeyword(std::initializer_list<Keyword> keywords) {
    if (std::find(keywords.begin(), keywords.end(), tokenizer.keyword()) == keywords.end()) {
        throw std::runtime_error("Keyword error at line " + std::to_string(tokenizer.lineNumber()));
    }
    tokenizer.advance();
}

void CompilationEngine::requireTokenType(TokenType tokenType) {
    if (tokenizer.tokenType() != tokenType) {
        throw std::runtime_error("Type error at line " + std::to_string(tokenizer.lineNumber()));
    }
    tokenizer.advance();
}
void CompilationEngine::requireTokenType(std::initializer_list<TokenType> tokenTypes) {
    if (std::find(tokenTypes.begin(), tokenTypes.end(), tokenizer.tokenType()) == tokenTypes.end()) {
        throw std::runtime_error("Type error at line " + std::to_string(tokenizer.lineNumber()));
    }
    tokenizer.advance();
}

CompilationEngine::~CompilationEngine() {}