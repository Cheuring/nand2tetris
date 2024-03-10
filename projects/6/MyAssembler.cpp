#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <bitset>

using namespace std;

void removeComments(string &line) {
    if (line.find("//") != string::npos) {
        line = line.substr(0, line.find("//"));
    }
}

void removeWhitespace(string &line) {
    line.erase(remove(line.begin(), line.end(), ' '), line.end());
}

class SymbolTable {
public:
    SymbolTable() {
        table["SP"] = 0;
        table["LCL"] = 1;
        table["ARG"] = 2;
        table["THIS"] = 3;
        table["THAT"] = 4;
        table["R0"] = 0;
        table["R1"] = 1;
        table["R2"] = 2;
        table["R3"] = 3;
        table["R4"] = 4;
        table["R5"] = 5;
        table["R6"] = 6;
        table["R7"] = 7;
        table["R8"] = 8;
        table["R9"] = 9;
        table["R10"] = 10;
        table["R11"] = 11;
        table["R12"] = 12;
        table["R13"] = 13;
        table["R14"] = 14;
        table["R15"] = 15;
        table["SCREEN"] = 16384;
        table["KBD"] = 24576;
    }

    void addEntry(string symbol, int address) {
        table[symbol] = address;
    }

    bool contains(string symbol) {
        return table.find(symbol) != table.end();
    }

    int getAddress(string symbol) {
        return table[symbol];
    }

private:
    map<string, int> table;
};

class Parser {
public:
    Parser(string filename) {
        file.open(filename);
    }

    bool hasMoreCommands() {
        return !file.eof();
    }

    void advance() {
        string line;
        getline(file, line);
        removeWhitespace(line);
        removeComments(line);
        while (line.length() == 0&& hasMoreCommands()) {
            advance();
            return;
        }
        currentCommand = line;
    }

    string commandType() {
        if (currentCommand[0] == '@') {
            return "A_COMMAND";
        } else if (currentCommand[0] == '(') {
            return "L_COMMAND";
        } else {
            return "C_COMMAND";
        }
    }

    string symbol() {
        if (commandType() == "A_COMMAND") {
            return currentCommand.substr(1);
        } else {
            return currentCommand.substr(1, currentCommand.length() - 2);
        }
    }

    string dest() {
        if (commandType() == "C_COMMAND") {
            if (currentCommand.find('=') != string::npos) {
                return currentCommand.substr(0, currentCommand.find('='));
            } else {
                return "";
            }
        } else {
            return "";
        }
    }

    string comp() {
        if (commandType() == "C_COMMAND") {
            if (currentCommand.find('=') != string::npos) {
                return currentCommand.substr(currentCommand.find('=') + 1);
            } else {
                return currentCommand.substr(0, currentCommand.find(';'));
            }
        } else {
            return "";
        }
    }

    string jump() {
        if (commandType() == "C_COMMAND") {
            if (currentCommand.find(';') != string::npos) {
                return currentCommand.substr(currentCommand.find(';') + 1);
            } else {
                return "";
            }
        } else {
            return "";
        }
    }

private:
    ifstream file;
    string currentCommand;
};

class Code {
public:
    Code(){
        destTable[""] = "000";
        destTable["M"] = "001";
        destTable["D"] = "010";
        destTable["MD"] = "011";
        destTable["A"] = "100";
        destTable["AM"] = "101";
        destTable["AD"] = "110";
        destTable["AMD"] = "111";

        compTable["0"] = "0101010";
        compTable["1"] = "0111111";
        compTable["-1"] = "0111010";
        compTable["D"] = "0001100";
        compTable["A"] = "0110000";
        compTable["!D"] = "0001101";
        compTable["!A"] = "0110001";
        compTable["-D"] = "0001111";
        compTable["-A"] = "0110011";
        compTable["D+1"] = "0011111";
        compTable["A+1"] = "0110111";
        compTable["D-1"] = "0001110";
        compTable["A-1"] = "0110010";
        compTable["D+A"] = "0000010";
        compTable["D-A"] = "0010011";
        compTable["A-D"] = "0000111";
        compTable["D&A"] = "0000000";
        compTable["D|A"] = "0010101";
        compTable["M"] = "1110000";
        compTable["!M"] = "1110001";
        compTable["-M"] = "1110011";
        compTable["M+1"] = "1110111";
        compTable["M-1"] = "1110010";
        compTable["D+M"] = "1000010";
        compTable["D-M"] = "1010011";
        compTable["M-D"] = "1000111";
        compTable["D&M"] = "1000000";
        compTable["D|M"] = "1010101";

        jumpTable[""] = "000";
        jumpTable["JGT"] = "001";
        jumpTable["JEQ"] = "010";
        jumpTable["JGE"] = "011";
        jumpTable["JLT"] = "100";
        jumpTable["JNE"] = "101";
        jumpTable["JLE"] = "110";
        jumpTable["JMP"] = "111";

    }

    string dest(string mnemonic) {
        return destTable[mnemonic];
    }

    string comp(string mnemonic) {
        return compTable[mnemonic];
    }

    string jump(string mnemonic) {
        return jumpTable[mnemonic];
    }

private:
    map<string, string> destTable;
    map<string, string> compTable;
    map<string, string> jumpTable;
};

int main(int argc, char *argv[]) {
    string filename = argv[1];
    // string filename = "Add.asm";
    string outputFilename = filename.substr(0, filename.rfind('.')) + ".hack";
    ofstream output(outputFilename);
    SymbolTable symbolTable;
    Parser parser(filename);
    Code code;
    int romAddress = 0;
    int ramAddress = 16;

    while (parser.hasMoreCommands()) {
        parser.advance();
        if(!parser.hasMoreCommands()) break;
        if (parser.commandType() == "L_COMMAND") {
            symbolTable.addEntry(parser.symbol(), romAddress);
        } else {
            romAddress++;
        }
    }

    parser = Parser(filename);
    while (parser.hasMoreCommands()) {
        parser.advance();
        if(!parser.hasMoreCommands()) break;
        if (parser.commandType() == "A_COMMAND") {
            string symbol = parser.symbol();
            int address;
            if (isdigit(symbol[0])) {
                address = stoi(symbol);
            } else {
                if (!symbolTable.contains(symbol)) {
                    symbolTable.addEntry(symbol, ramAddress);
                    ramAddress++;
                }
                address = symbolTable.getAddress(symbol);
            }
            output << "0" << bitset<15>(address).to_string() << endl;
        } else if (parser.commandType() == "C_COMMAND") {
            output << "111" << code.comp(parser.comp()) << code.dest(parser.dest()) << code.jump(parser.jump()) << endl;
        }
    }
    output.close();
    return 0;
}
