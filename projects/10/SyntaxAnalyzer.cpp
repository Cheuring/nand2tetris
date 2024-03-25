#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

enum class TokenType {
    KEYWORD,
    SYMBOL,
    IDENTIFIER,
    INT_CONST,
    STRING_CONST
};

enum class Keyword {
    CLASS,
    METHOD,
    FUNCTION,
    CONSTRUCTOR,
    INT,
    BOOLEAN,
    CHAR,
    VOID,
    VAR,
    STATIC,
    FIELD,
    LET,
    DO,
    IF,
    ELSE,
    WHILE,
    RETURN,
    TRUE,
    FALSE,
    NULL_,
    THIS
};

class JackTokenizer {
public:
    JackTokenizer(string &filename): file(filename) , _countLine(0) {
        advance();
    }

    bool hasMoreTokens() {
        return !file.eof();
    }

    bool advance() {
        currentToken.clear();
        removeSpace(line);
        while(line.empty()&&hasMoreTokens()){
            getline(file,line);
            _countLine++;
            removeComments(line);
            removeSpace(line);
            if(line.find("/*")!=string::npos){
                string temp;
                while(line.find("*/")==string::npos){
                    getline(file,temp);
                    _countLine++;
                    line+=temp;
                }
                line=line.substr(0,line.find("/*"))+line.substr(line.find("*/")+2);
            }
        }
        
        istringstream iss(line);
        iss>>currentToken;
        string symbols = ".,()[];+-*/&|<>=~\"{}";
        size_t pos = currentToken.find_first_of(symbols);
        if (pos != string::npos) {
            if(currentToken[pos]=='"'){
                size_t pos1=line.find_first_of('"');
                size_t pos2=line.find('"',pos1+1);
                currentToken = line.substr(pos1,pos2-pos1+1);
            }else{
                string temp;
                temp = currentToken.substr(0, pos);
                currentToken = temp.empty() ? currentToken.substr(0,1) : temp;
            }
        }
        line = line.substr(line.find(currentToken)+currentToken.length());
        return !currentToken.empty();
    }

    TokenType tokenType() {
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

    Keyword keyWord() {
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

    char symbol() {
        return currentToken[0];
    }

    string identifier() {
        return currentToken;
    }

    int intVal() {
        return stoi(currentToken);
    }

    string stringVal() {
        return currentToken.substr(1,currentToken.length()-2);
    }

    size_t countLine() {
        return _countLine;
    }

    ~JackTokenizer() {
        file.close();
    }

private:
    void removeComments(string& line){
        int pos;
        if((pos=line.find("//"))!=string::npos){
            line=line.substr(0,pos);
        }
    }
    
    void removeSpace(string &line){
        line.erase(line.begin(),find_if(line.begin(),line.end(),[](unsigned char ch){
            return !isspace(ch);
        }));
    }

    string currentToken;
    string line;
    ifstream file;
    size_t _countLine;
};

class CompilationEngine {   
public:
    CompilationEngine(string &infilename,string &outfilename): tokenizer(infilename), file(outfilename) {
        file<<"<tokens>"<<"\n";
        compileClass();
    }

    void compileClass() {
        file<<"<keyword> class </keyword>"<<"\n";
        tokenizer.advance();
        file<<"<identifier> "<<tokenizer.identifier()<<" </identifier>"<<"\n";
        tokenizer.advance();
        requireSymbol('{');
        while(tokenizer.tokenType()==TokenType::KEYWORD&&
              (tokenizer.keyWord()==Keyword::STATIC||tokenizer.keyWord()==Keyword::FIELD)){
            compileClassVarDec();
        }
        while(tokenizer.tokenType()==TokenType::KEYWORD&&
              (tokenizer.keyWord()==Keyword::CONSTRUCTOR||tokenizer.keyWord()==Keyword::FUNCTION||
               tokenizer.keyWord()==Keyword::METHOD)){
            compileSubroutine();
        }
        requireSymbol('}');
    }

    void compileClassVarDec() {
        file<<"<keyword> "<<(tokenizer.keyWord()==Keyword::STATIC?"static":"field")<<" </keyword>"<<"\n";
        tokenizer.advance();
        if(tokenizer.tokenType()==TokenType::KEYWORD){
            file<<"<keyword> "<<(tokenizer.keyWord()==Keyword::INT?"int":(tokenizer.keyWord()==Keyword::CHAR?"char":"boolean"))<<" </keyword>"<<"\n";
        }else{
            file<<"<identifier> "<<tokenizer.identifier()<<" </identifier>"<<"\n";
        }
        tokenizer.advance();
        file<<"<identifier> "<<tokenizer.identifier()<<" </identifier>"<<"\n";
        tokenizer.advance();
        while(tokenizer.symbol()==','){
            file<<"<symbol> , </symbol>"<<"\n";
            tokenizer.advance();
            file<<"<identifier> "<<tokenizer.identifier()<<" </identifier>"<<"\n";
            tokenizer.advance();
        }
        requireSymbol(';');
    }

    void compileSubroutine() {
        file<<"<keyword> "<<(tokenizer.keyWord()==Keyword::CONSTRUCTOR?"constructor":(tokenizer.keyWord()==Keyword::FUNCTION?"function":"method"))<<" </keyword>"<<"\n";
        tokenizer.advance();
        if(tokenizer.tokenType()==TokenType::KEYWORD){
            file<<"<keyword> "<<(tokenizer.keyWord()==Keyword::INT?"int":(tokenizer.keyWord()==Keyword::CHAR?"char":(tokenizer.keyWord()==Keyword::VOID?"void":"boolean")))<<" </keyword>"<<"\n";
        }else{
            file<<"<identifier> "<<tokenizer.identifier()<<" </identifier>"<<"\n";
        }
        tokenizer.advance();
        file<<"<identifier> "<<tokenizer.identifier()<<" </identifier>"<<"\n";
        tokenizer.advance();
        requireSymbol('(');
        compileParameterList();
        requireSymbol(')');
        compileSubroutineBody();
    }

    void compileParameterList() {
        if(tokenizer.symbol()!=')'){
            file<<"<keyword> "<<(tokenizer.keyWord()==Keyword::INT?"int":(tokenizer.keyWord()==Keyword::CHAR?"char":"boolean"))<<" </keyword>"<<"\n";
            tokenizer.advance();
            file<<"<identifier> "<<tokenizer.identifier()<<" </identifier>"<<"\n";
            tokenizer.advance();
            while(tokenizer.symbol()==','){
                file<<"<symbol> , </symbol>"<<"\n";
                tokenizer.advance();
                file<<"<keyword> "<<(tokenizer.keyWord()==Keyword::INT?"int":(tokenizer.keyWord()==Keyword::CHAR?"char":"boolean"))<<" </keyword>"<<"\n";
                tokenizer.advance();
                file<<"<identifier> "<<tokenizer.identifier()<<" </identifier>"<<"\n";
                tokenizer.advance();
            }
        }
    }

    void compileSubroutineBody() {
        requireSymbol('{');
        while(tokenizer.tokenType()==TokenType::KEYWORD&&tokenizer.keyWord()==Keyword::VAR){
            compileVarDec();
        }
        compileStatements();
        requireSymbol('}');
    }

    void compileVarDec() {
        file<<"<keyword> var </keyword>"<<"\n";
        tokenizer.advance();
        if(tokenizer.tokenType()==TokenType::KEYWORD){
            file<<"<keyword> "<<(tokenizer.keyWord()==Keyword::INT?"int":(tokenizer.keyWord()==Keyword::CHAR?"char":"boolean"))<<" </keyword>"<<"\n";
        }else{
            file<<"<identifier> "<<tokenizer.identifier()<<" </identifier>"<<"\n";
        }
        tokenizer.advance();
        file<<"<identifier> "<<tokenizer.identifier()<<" </identifier>"<<"\n";
        tokenizer.advance();
        while(tokenizer.symbol()==','){
            file<<"<symbol> , </symbol>"<<"\n";
            tokenizer.advance();
            file<<"<identifier> "<<tokenizer.identifier()<<" </identifier>"<<"\n";
            tokenizer.advance();
        }
        requireSymbol(';');
    }

    void compileStatements() {
        while(tokenizer.tokenType()==TokenType::KEYWORD){
            if(tokenizer.keyWord()==Keyword::LET){
                compileLet();
            }else if(tokenizer.keyWord()==Keyword::IF){
                compileIf();
            }else if(tokenizer.keyWord()==Keyword::WHILE){
                compileWhile();
            }else if(tokenizer.keyWord()==Keyword::DO){
                compileDo();
            }else if(tokenizer.keyWord()==Keyword::RETURN){
                compileReturn();
            }
        }
    }

    void compileLet() {
        file<<"<keyword> let </keyword>"<<"\n";
        tokenizer.advance();
        file<<"<identifier> "<<tokenizer.identifier()<<" </identifier>"<<"\n";
        tokenizer.advance();
        if(tokenizer.symbol()=='['){
            file<<"<symbol> [ </symbol>"<<"\n";
            tokenizer.advance();
            compileExpression();
            requireSymbol(']');
        }
        file<<"<symbol> = </symbol>"<<"\n";
        tokenizer.advance();
        compileExpression();
        requireSymbol(';');
    }

    void compileIf() {
        file<<"<keyword> if </keyword>"<<"\n";
        tokenizer.advance();
        requireSymbol('(');
        compileExpression();
        requireSymbol(')');
        requireSymbol('{');
        compileStatements();
        requireSymbol('}');
        if(tokenizer.tokenType()==TokenType::KEYWORD&&tokenizer.keyWord()==Keyword::ELSE){
            file<<"<keyword> else </keyword>"<<"\n";
            tokenizer.advance();
            requireSymbol('{');
            compileStatements();
            requireSymbol('}');
        }
    }

    void compileWhile() {
        file<<"<keyword> while </keyword>"<<"\n";
        tokenizer.advance();
        requireSymbol('(');
        compileExpression();
        requireSymbol(')');
        requireSymbol('{');
        compileStatements();
        requireSymbol('}');
    }

    void compileDo() {
        file<<"<keyword> do </keyword>"<<"\n";
        tokenizer.advance();
        compileSubroutineCall();
        requireSymbol(';');
    }

    void compileReturn() {
        file<<"<keyword> return </keyword>"<<"\n";
        tokenizer.advance();
        if(tokenizer.tokenType()!=TokenType::SYMBOL){
            compileExpression();
        }
        requireSymbol(';');
    }

    void compileExpression() {
        compileTerm();
        while(tokenizer.symbol()=='+'||tokenizer.symbol()=='-'||tokenizer.symbol()=='*'||
              tokenizer.symbol()=='/'||tokenizer.symbol()=='&'||tokenizer.symbol()=='|'||
              tokenizer.symbol()=='<'||tokenizer.symbol()=='>'||tokenizer.symbol()=='='){
            if(tokenizer.symbol()=='<'){
                file<<"<symbol> &lt; </symbol>"<<"\n";
            }else if(tokenizer.symbol()=='>'){
                file<<"<symbol> &gt; </symbol>"<<"\n";
            }else if(tokenizer.symbol()=='&'){
                file<<"<symbol> &amp; </symbol>"<<"\n";
            }else{
                file<<"<symbol> "<<tokenizer.symbol()<<" </symbol>"<<"\n";
            }
            tokenizer.advance();
            compileTerm();
        }
    }

    void compileTerm() {
        if(tokenizer.tokenType()==TokenType::INT_CONST){
            file<<"<integerConstant> "<<tokenizer.intVal()<<" </integerConstant>"<<"\n";
            tokenizer.advance();
        }else if(tokenizer.tokenType()==TokenType::STRING_CONST){
            file<<"<stringConstant> "<<tokenizer.stringVal()<<" </stringConstant>"<<"\n";
            tokenizer.advance();
        }else if(tokenizer.tokenType()==TokenType::KEYWORD&&
                 (tokenizer.keyWord()==Keyword::TRUE||tokenizer.keyWord()==Keyword::FALSE||
                  tokenizer.keyWord()==Keyword::NULL_||tokenizer.keyWord()==Keyword::THIS)){
            file<<"<keyword> "<<(tokenizer.keyWord()==Keyword::TRUE?"true":(tokenizer.keyWord()==Keyword::FALSE?"false":(tokenizer.keyWord()==Keyword::NULL_?"null":"this")))<<" </keyword>"<<"\n";
            tokenizer.advance();
        }else if(tokenizer.tokenType()==TokenType::IDENTIFIER){
            file<<"<identifier> "<<tokenizer.identifier()<<" </identifier>"<<"\n";
            tokenizer.advance();
            if(tokenizer.symbol()=='['){
                file<<"<symbol> [ </symbol>"<<"\n";
                tokenizer.advance();
                compileExpression();
                requireSymbol(']');
            }else if(tokenizer.symbol()=='('||tokenizer.symbol()=='.'){
                compileSubroutineCall();
            }
        }else if(tokenizer.symbol()=='('){
            file<<"<symbol> ( </symbol>"<<"\n";
            tokenizer.advance();
            compileExpression();
            requireSymbol(')');
        }else if(tokenizer.symbol()=='-'||tokenizer.symbol()=='~'){
            file<<"<symbol> "<<tokenizer.symbol()<<" </symbol>"<<"\n";
            tokenizer.advance();
            compileTerm();
        }
    }

    void compileSubroutineCall() {
        if(tokenizer.tokenType()!=TokenType::SYMBOL){
            file<<"<identifier> "<<tokenizer.identifier()<<" </identifier>"<<"\n";
            tokenizer.advance();
        }
        if(tokenizer.symbol()=='.'){
            file<<"<symbol> . </symbol>"<<"\n";
            tokenizer.advance();
            file<<"<identifier> "<<tokenizer.identifier()<<" </identifier>"<<"\n";
            tokenizer.advance();
        }
        requireSymbol('(');
        compileExpressionList();
        requireSymbol(')');
    }

    void compileExpressionList() {
        compileExpression();
        while(tokenizer.symbol()==','){
            file<<"<symbol> , </symbol>"<<"\n";
            tokenizer.advance();
            compileExpression();
        }
    }

    ~CompilationEngine() {
        file<<"</tokens>"<<"\n";
        file.close();
    }

private:
    void requireSymbol(char symbol) {
        if (tokenizer.symbol() != symbol) {
            throw runtime_error("Expected symbol: " + string(1, symbol) + " at line: " + to_string(tokenizer.countLine()));
        } else {
            file << "<symbol> " << symbol << " </symbol>" << "\n";
            tokenizer.advance();
        }
    }

    JackTokenizer tokenizer;
    ofstream file;
};

int main(int argc, char **argv){
    string infilename = argv[1];
    // string infilename = "./ExpressionLessSquare/Square.jack";
    string outfilename = infilename.substr(0,infilename.find_last_of('.'))+"_test.xml";
    JackTokenizer tokenizer(infilename);
    CompilationEngine compilationEngine(infilename,outfilename);
    return 0;
}