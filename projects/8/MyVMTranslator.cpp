#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <algorithm>
using namespace std;

enum CommandTypes{
    FAILURE,
    C_ARITHMETIC,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL
};

class Parser {
public:
    Parser(const string &filename){
        file.open(filename);
    }

    bool hasMoreCommands(){
        return !file.eof();
    }

    bool advance(){
        string line;
        do{
            getline(file,line);
            removeComments(line);
            // remove leading and trailing whitespaces
            line.erase(line.begin(),find_if(line.begin(),line.end(),[](unsigned char ch){
                return !isspace(ch);
            }));
        }while(line.empty()&&hasMoreCommands());
        currentCommand=segment_=index_="";
        istringstream iss(line);
        iss>>currentCommand;
        iss>>segment_;
        iss>>index_;
        return !currentCommand.empty();
    }

    CommandTypes CommandType(){
        if(segment_==""){
            if(currentCommand=="return") return C_RETURN;
            return C_ARITHMETIC;
        }
        if(currentCommand=="push") return C_PUSH;
        if(currentCommand=="pop") return C_POP;
        if(currentCommand=="label") return C_LABEL;
        if(currentCommand=="goto") return C_GOTO;
        if(currentCommand=="if-goto") return C_IF;
        if(currentCommand=="function") return C_FUNCTION;
        if(currentCommand=="call") return C_CALL;
        return FAILURE;
    }

    string arg1(){
        if(CommandType()==C_ARITHMETIC) return currentCommand;
        else return segment_;
    }

    int arg2(){
        return stoi(index_);
    }

    ~Parser(){
        file.close();
    }

private:
    void removeComments(string& line){
        int pos;
        if((pos=line.find("//"))!=string::npos){
            line=line.substr(0,pos);
        }
    }

    string currentCommand;
    string segment_;
    string index_;
    ifstream file;
};

class CodeWriter {
public:
    CodeWriter(const string &filename){
        file.open(filename);
        filename_=filename;
    }

    void setFileName(const string &filename){
        filename_=filename;
    }

    void writeInit(){
        file << "// Bootstrap code" << "\n";
        file<<"@256"<<"\n";
        file<<"D=A"<<"\n";
        file<<"@SP"<<"\n";
        file<<"M=D"<<"\n";
        
        writeCall("Sys.init",0);
    }

    void writeArithmetic(const string &command){
        file<<"// "<<command<<"\n";
        if(command=="add"){
            file<<"@SP"<<"\n";
            file<<"AM=M-1"<<"\n";
            file<<"D=M"<<"\n";
            file<<"A=A-1"<<"\n";
            file<<"M=M+D"<<"\n";
        }else if(command=="sub"){
            file<<"@SP"<<"\n";
            file<<"AM=M-1"<<"\n";
            file<<"D=M"<<"\n";
            file<<"A=A-1"<<"\n";
            file<<"M=M-D"<<"\n";
        }else if(command=="neg"){
            file<<"@SP"<<"\n";
            file<<"A=M-1"<<"\n";
            file<<"M=-M"<<"\n";
        }else if(command=="eq"){
            file<<"@SP"<<"\n";
            file<<"AM=M-1"<<"\n";
            file<<"D=M"<<"\n";
            file<<"A=A-1"<<"\n";
            file<<"D=M-D"<<"\n";
            file<<"@EQ_TRUE"<<eqCounter<<"\n";
            file<<"D;JEQ"<<"\n";
            file<<"@SP"<<"\n";
            file<<"A=M-1"<<"\n";
            file<<"M=0"<<"\n";
            file<<"@EQ_END"<<eqCounter<<"\n";
            file<<"0;JMP"<<"\n";
            file<<"(EQ_TRUE"<<eqCounter<<")"<<"\n";
            file<<"@SP"<<"\n";
            file<<"A=M-1"<<"\n";
            file<<"M=-1"<<"\n";
            file<<"(EQ_END"<<eqCounter<<")"<<"\n";
            eqCounter++;
        }else if(command=="gt"){
            file<<"@SP"<<"\n";
            file<<"AM=M-1"<<"\n";
            file<<"D=M"<<"\n";
            file<<"A=A-1"<<"\n";
            file<<"D=M-D"<<"\n";
            file<<"@GT_TRUE"<<gtCounter<<"\n";
            file<<"D;JGT"<<"\n";
            file<<"@SP"<<"\n";
            file<<"A=M-1"<<"\n";
            file<<"M=0"<<"\n";
            file<<"@GT_END"<<gtCounter<<"\n";
            file<<"0;JMP"<<"\n";
            file<<"(GT_TRUE"<<gtCounter<<")"<<"\n";
            file<<"@SP"<<"\n";
            file<<"A=M-1"<<"\n";
            file<<"M=-1"<<"\n";
            file<<"(GT_END"<<gtCounter<<")"<<"\n";
            gtCounter++;
        }else if(command=="lt"){
            file<<"@SP"<<"\n";
            file<<"AM=M-1"<<"\n";
            file<<"D=M"<<"\n";
            file<<"A=A-1"<<"\n";
            file<<"D=M-D"<<"\n";
            file<<"@LT_TRUE"<<ltCounter<<"\n";
            file<<"D;JLT"<<"\n";
            file<<"@SP"<<"\n";
            file<<"A=M-1"<<"\n";
            file<<"M=0"<<"\n";
            file<<"@LT_END"<<ltCounter<<"\n";
            file<<"0;JMP"<<"\n";
            file<<"(LT_TRUE"<<ltCounter<<")"<<"\n";
            file<<"@SP"<<"\n";
            file<<"A=M-1"<<"\n";
            file<<"M=-1"<<"\n";
            file<<"(LT_END"<<ltCounter<<")"<<"\n";
            ltCounter++;
        }else if(command=="and"){
            file<<"@SP"<<"\n";
            file<<"AM=M-1"<<"\n";
            file<<"D=M"<<"\n";
            file<<"A=A-1"<<"\n";
            file<<"M=M&D"<<"\n";
        }else if(command=="or"){
            file<<"@SP"<<"\n";
            file<<"AM=M-1"<<"\n";
            file<<"D=M"<<"\n";
            file<<"A=A-1"<<"\n";
            file<<"M=M|D"<<"\n";
        }else if(command=="not"){
            file<<"@SP"<<"\n";
            file<<"A=M-1"<<"\n";
            file<<"M=!M"<<"\n";
        }
    }

    void writePushPop(CommandTypes command, const string& segment, int index){
        if(command == C_PUSH){
            if(segment == "constant"){
                file << "// push constant " << index << "\n";
                file << "@" << index << "\n";
                file << "D=A" << "\n";
                file << "@SP" << "\n";
                file << "A=M" << "\n";
                file << "M=D" << "\n";
                file << "@SP" << "\n";
                file << "M=M+1" << "\n";
            }else if(segment == "local"){
                file << "// push local " << index << "\n";
                file << "@" << index << "\n";
                file << "D=A" << "\n";
                file << "@LCL" << "\n";
                file << "A=M+D" << "\n";
                file << "D=M" << "\n";
                file << "@SP" << "\n";
                file << "A=M" << "\n";
                file << "M=D" << "\n";
                file << "@SP" << "\n";
                file << "M=M+1" << "\n";
            }else if(segment == "argument"){
                file << "// push argument " << index << "\n";
                file << "@" << index << "\n";
                file << "D=A" << "\n";
                file << "@ARG" << "\n";
                file << "A=M+D" << "\n";
                file << "D=M" << "\n";
                file << "@SP" << "\n";
                file << "A=M" << "\n";
                file << "M=D" << "\n";
                file << "@SP" << "\n";
                file << "M=M+1" << "\n";
            }else if(segment == "this"){
                file << "// push this " << index << "\n";
                file << "@" << index << "\n";
                file << "D=A" << "\n";
                file << "@THIS" << "\n";
                file << "A=M+D" << "\n";
                file << "D=M" << "\n";
                file << "@SP" << "\n";
                file << "A=M" << "\n";
                file << "M=D" << "\n";
                file << "@SP" << "\n";
                file << "M=M+1" << "\n";
            }else if(segment == "that"){
                file << "// push that " << index << "\n";
                file << "@" << index << "\n";
                file << "D=A" << "\n";
                file << "@THAT" << "\n";
                file << "A=M+D" << "\n";
                file << "D=M" << "\n";
                file << "@SP" << "\n";
                file << "A=M" << "\n";
                file << "M=D" << "\n";
                file << "@SP" << "\n";
                file << "M=M+1" << "\n";
            }else if(segment == "temp"){
                file << "// push temp " << index << "\n";
                file << "@" << index << "\n";
                file << "D=A" << "\n";
                file << "@5" << "\n";
                file << "A=A+D" << "\n";
                file << "D=M" << "\n";
                file << "@SP" << "\n";
                file << "A=M" << "\n";
                file << "M=D" << "\n";
                file << "@SP" << "\n";
                file << "M=M+1" << "\n";
            }else if(segment == "static"){
                file << "// push static " << index << "\n";
                file << "@" << filename_ << "." << index << "\n";
                file << "D=M" << "\n";
                file << "@SP" << "\n";
                file << "A=M" << "\n";
                file << "M=D" << "\n";
                file << "@SP" << "\n";
                file << "M=M+1" << "\n";
            }else if(segment == "pointer"){
                if(index == 0){
                    file << "// push pointer 0" << "\n";
                    file << "@THIS" << "\n";
                    file << "D=M" << "\n";
                    file << "@SP" << "\n";
                    file << "A=M" << "\n";
                    file << "M=D" << "\n";
                    file << "@SP" << "\n";
                    file << "M=M+1" << "\n";
                }else if(index == 1){
                    file << "// push pointer 1" << "\n";
                    file << "@THAT" << "\n";
                    file << "D=M" << "\n";
                    file << "@SP" << "\n";
                    file << "A=M" << "\n";
                    file << "M=D" << "\n";
                    file << "@SP" << "\n";
                    file << "M=M+1" << "\n";
                }
            }
        }
        else if(command == C_POP){
            if(segment == "constant"){
                // Invalid operation, do nothing
            }else if(segment == "local"){
                file << "// pop local " << index << "\n";
                file << "@" << index << "\n";
                file << "D=A" << "\n";
                file << "@LCL" << "\n";
                file << "D=M+D" << "\n";
                file << "@R13" << "\n";
                file << "M=D" << "\n";
                file << "@SP" << "\n";
                file << "AM=M-1" << "\n";
                file << "D=M" << "\n";
                file << "@R13" << "\n";
                file << "A=M" << "\n";
                file << "M=D" << "\n";
            }else if(segment == "argument"){
                file << "// pop argument " << index << "\n";
                file << "@" << index << "\n";
                file << "D=A" << "\n";
                file << "@ARG" << "\n";
                file << "D=M+D" << "\n";
                file << "@R13" << "\n";
                file << "M=D" << "\n";
                file << "@SP" << "\n";
                file << "AM=M-1" << "\n";
                file << "D=M" << "\n";
                file << "@R13" << "\n";
                file << "A=M" << "\n";
                file << "M=D" << "\n";
            }else if(segment == "this"){
                file << "// pop this " << index << "\n";
                file << "@" << index << "\n";
                file << "D=A" << "\n";
                file << "@THIS" << "\n";
                file << "D=M+D" << "\n";
                file << "@R13" << "\n";
                file << "M=D" << "\n";
                file << "@SP" << "\n";
                file << "AM=M-1" << "\n";
                file << "D=M" << "\n";
                file << "@R13" << "\n";
                file << "A=M" << "\n";
                file << "M=D" << "\n";
            }else if(segment == "that"){
                file << "// pop that " << index << "\n";
                file << "@" << index << "\n";
                file << "D=A" << "\n";
                file << "@THAT" << "\n";
                file << "D=M+D" << "\n";
                file << "@R13" << "\n";
                file << "M=D" << "\n";
                file << "@SP" << "\n";
                file << "AM=M-1" << "\n";
                file << "D=M" << "\n";
                file << "@R13" << "\n";
                file << "A=M" << "\n";
                file << "M=D" << "\n";
            }else if(segment == "temp"){
            file << "// pop temp " << index << "\n";
            file << "@" << index << "\n";
            file << "D=A" << "\n";
            file << "@5" << "\n";
            file << "D=A+D" << "\n";
            file << "@R13" << "\n";
            file << "M=D" << "\n";
            file << "@SP" << "\n";
            file << "AM=M-1" << "\n";
            file << "D=M" << "\n";
            file << "@R13" << "\n";
            file << "A=M" << "\n";
            file << "M=D" << "\n";
            }else if(segment == "static"){
                file << "// pop static " << index << "\n";
                file << "@SP" << "\n";
                file << "AM=M-1" << "\n";
                file << "D=M" << "\n";
                file << "@" << filename_ << "." << index << "\n";
                file << "M=D" << "\n";
            }else if(segment == "pointer"){
                if(index == 0){
                    file << "// pop pointer 0" << "\n";
                    file << "@SP" << "\n";
                    file << "AM=M-1" << "\n";
                    file << "D=M" << "\n";
                    file << "@THIS" << "\n";
                    file << "M=D" << "\n";
                }else if(index == 1){
                    file << "// pop pointer 1" << "\n";
                    file << "@SP" << "\n";
                    file << "AM=M-1" << "\n";
                    file << "D=M" << "\n";
                    file << "@THAT" << "\n";
                    file << "M=D" << "\n";
                }
            }
        }
    }

    void writeLabel(const string &label){
        file << "// label " << label << "\n";
        file<<"("<<label<<")"<<"\n";
    }

    void writeGoto(const string &label){
        file << "// goto " << label << "\n";
        file<<"@"<<label<<"\n";
        file<<"0;JMP"<<"\n";
    }

    void writeIf(const string &label){
        file << "// if-goto " << label << "\n";
        file<<"@SP"<<"\n";
        file<<"AM=M-1"<<"\n";
        file<<"D=M"<<"\n";
        file<<"@"<<label<<"\n";
        file<<"D;JNE"<<"\n";
    }

    void writeFunction(const string &functionName, int numLocals){
        file << "// function " << functionName << " " << numLocals << "\n";
        file<<"("<<functionName<<")"<<"\n";
        for(int i=0;i<numLocals;i++){
            writePushPop(C_PUSH,"constant",0);
        }
    }

    void writeCall(const string &functionName, int numArgs){
        file << "// call " << functionName << " " << numArgs << "\n";
        file<<"@RETURN_ADDRESS"<<callCounter<<"\n";
        file<<"D=A"<<"\n";
        file<<"@SP"<<"\n";
        file<<"A=M"<<"\n";
        file<<"M=D"<<"\n";
        file<<"@SP"<<"\n";
        file<<"M=M+1"<<"\n";
        file<<"@LCL"<<"\n";
        file<<"D=M"<<"\n";
        file<<"@SP"<<"\n";
        file<<"A=M"<<"\n";
        file<<"M=D"<<"\n";
        file<<"@SP"<<"\n";
        file<<"M=M+1"<<"\n";
        file<<"@ARG"<<"\n";
        file<<"D=M"<<"\n";
        file<<"@SP"<<"\n";
        file<<"A=M"<<"\n";
        file<<"M=D"<<"\n";
        file<<"@SP"<<"\n";
        file<<"M=M+1"<<"\n";
        file<<"@THIS"<<"\n";
        file<<"D=M"<<"\n";
        file<<"@SP"<<"\n";
        file<<"A=M"<<"\n";
        file<<"M=D"<<"\n";
        file<<"@SP"<<"\n";
        file<<"M=M+1"<<"\n";
        file<<"@THAT"<<"\n";
        file<<"D=M"<<"\n";
        file<<"@SP"<<"\n";
        file<<"A=M"<<"\n";
        file<<"M=D"<<"\n";
        file<<"@SP"<<"\n";
        file<<"M=M+1"<<"\n";
        file<<"@SP"<<"\n";
        file<<"D=M"<<"\n";
        file<<"@"<<numArgs<<"\n";
        file<<"D=D-A"<<"\n";
        file<<"@5"<<"\n";
        file<<"D=D-A"<<"\n";
        file<<"@ARG"<<"\n";
        file<<"M=D"<<"\n";
        file<<"@SP"<<"\n";
        file<<"D=M"<<"\n";
        file<<"@LCL"<<"\n";
        file<<"M=D"<<"\n";
        file<<"@"<<functionName<<"\n";
        file<<"0;JMP"<<"\n";
        file<<"(RETURN_ADDRESS"<<callCounter<<")"<<"\n";
        callCounter++;
    }

    void writeReturn(){
        file << "// return" << "\n";
        file<<"@LCL"<<"\n";
        file<<"D=M"<<"\n";
        file<<"@R13"<<"\n";
        file<<"M=D"<<"\n";
        file<<"@5"<<"\n";
        file<<"A=D-A"<<"\n";
        file<<"D=M"<<"\n";
        file<<"@R14"<<"\n";
        file<<"M=D"<<"\n";
        file<<"@SP"<<"\n";
        file<<"AM=M-1"<<"\n";
        file<<"D=M"<<"\n";
        file<<"@ARG"<<"\n";
        file<<"A=M"<<"\n";
        file<<"M=D"<<"\n";
        file<<"@ARG"<<"\n";
        file<<"D=M+1"<<"\n";
        file<<"@SP"<<"\n";
        file<<"M=D"<<"\n";
        file<<"@R13"<<"\n";
        file<<"AM=M-1"<<"\n";
        file<<"D=M"<<"\n";
        file<<"@THAT"<<"\n";
        file<<"M=D"<<"\n";
        file<<"@R13"<<"\n";
        file<<"AM=M-1"<<"\n";
        file<<"D=M"<<"\n";
        file<<"@THIS"<<"\n";
        file<<"M=D"<<"\n";
        file<<"@R13"<<"\n";
        file<<"AM=M-1"<<"\n";
        file<<"D=M"<<"\n";
        file<<"@ARG"<<"\n";
        file<<"M=D"<<"\n";
        file<<"@R13"<<"\n";
        file<<"AM=M-1"<<"\n";
        file<<"D=M"<<"\n";
        file<<"@LCL"<<"\n";
        file<<"M=D"<<"\n";
        file<<"@R14"<<"\n";
        file<<"A=M"<<"\n";
        file<<"0;JMP"<<"\n";
    }

    ~CodeWriter(){
        file.close();
    }
private:
    ofstream file;
    string filename_;
    int callCounter=0;
    int eqCounter=0;
    int gtCounter=0;
    int ltCounter=0;
};

int main(int argc,char* argv[]){
    string path=argv[1];
    // string path="./FunctionCalls/StaticsTest";

    if(filesystem::is_directory(path)){
        filesystem::path p(path);
        string fileout=(p/p.filename()).string()+".asm";
        CodeWriter codeWriter(fileout);
        codeWriter.writeInit(); 
        for(const auto& entry:filesystem::directory_iterator(path)){
            if(entry.path().extension()==".vm"){
                string filein=entry.path().string();
                Parser parser(filein);
                codeWriter.setFileName(entry.path().stem().string());
                while(parser.hasMoreCommands()){
                    if(!parser.advance()) break;

                    CommandTypes commandType=parser.CommandType();
                    switch (commandType)
                    {
                        case C_ARITHMETIC:
                            codeWriter.writeArithmetic(parser.arg1());
                            break;
                        case C_PUSH:
                        case C_POP:
                            codeWriter.writePushPop(commandType,parser.arg1(),parser.arg2());
                            break;
                        case C_LABEL:
                            codeWriter.writeLabel(parser.arg1());
                            break;
                        case C_GOTO:
                            codeWriter.writeGoto(parser.arg1());
                            break;
                        case C_IF:
                            codeWriter.writeIf(parser.arg1());
                            break;
                        case C_FUNCTION:
                            codeWriter.writeFunction(parser.arg1(),parser.arg2());
                            break;
                        case C_RETURN:
                            codeWriter.writeReturn();
                            break;
                        case C_CALL:
                            codeWriter.writeCall(parser.arg1(),parser.arg2());
                            break;
                        default:
                            break;
                    }

                }
            }
        }
    }else if(filesystem::is_regular_file(path)){
        string filein=path;
        string fileout=filein.substr(0,filein.rfind('.'))+".asm";
        Parser parser(filein);
        CodeWriter codeWriter(fileout);
        // codeWriter.writeInit(); //it shuold be commented if there is no Sys.init function
        while(parser.hasMoreCommands()){
            if(!parser.advance()) break;

            CommandTypes commandType=parser.CommandType();
            switch (commandType)
            {
                case C_ARITHMETIC:
                    codeWriter.writeArithmetic(parser.arg1());
                    break;
                case C_PUSH:
                case C_POP:
                    codeWriter.writePushPop(commandType,parser.arg1(),parser.arg2());
                    break;
                case C_LABEL:
                    codeWriter.writeLabel(parser.arg1());
                    break;
                case C_GOTO:
                    codeWriter.writeGoto(parser.arg1());
                    break;
                case C_IF:
                    codeWriter.writeIf(parser.arg1());
                    break;
                case C_FUNCTION:
                    codeWriter.writeFunction(parser.arg1(),parser.arg2());
                    break;
                case C_RETURN:
                    codeWriter.writeReturn();
                    break;
                case C_CALL:
                    codeWriter.writeCall(parser.arg1(),parser.arg2());
                    break;
                default:
                    break;
            }
            
        }
    }else cout<<"Invalid path"<<endl;

    return 0;
}