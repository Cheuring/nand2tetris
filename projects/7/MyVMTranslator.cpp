#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

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

    void advance(){
        string line;
        do{
            getline(file,line);
            removeComments(line);
        }while(line.empty()&&hasMoreCommands());
        currentCommand=segment_=index_="";
        istringstream iss(line);
        iss>>currentCommand;
        iss>>segment_;
        iss>>index_;

    }

    //only considered the elements we need in project7
    CommandTypes CommandType(){
        if(segment_=="") return C_ARITHMETIC;
        if(currentCommand=="push") return C_PUSH;
        if(currentCommand=="pop") return C_POP;
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
                file << "@" << "MyVMtranslator." << index << "\n";
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
                file << "@" << "MyVMtranslator." << index << "\n";
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

    ~CodeWriter(){
        file.close();
    }
private:
    ofstream file;
    int eqCounter=0;
    int gtCounter=0;
    int ltCounter=0;
};

int main(int argc,char* argv[]){
    string filein=argv[1];
    // string filein=".\\MemoryAccess\\BasicTest\\BasicTest.vm";
    string fileout=filein.substr(0,filein.rfind('.'))+".asm";
    Parser parser(filein);
    CodeWriter codeWriter(fileout);

    while(parser.hasMoreCommands()){
        parser.advance();
        if(!parser.hasMoreCommands()) break;

        if(parser.CommandType()==C_ARITHMETIC) codeWriter.writeArithmetic(parser.arg1());
        else codeWriter.writePushPop(parser.CommandType(),parser.arg1(),parser.arg2());
    }

    return 0;
}