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
    string dest(string mnemonic) {
        if (mnemonic == "") {
            return "000";
        } else if (mnemonic == "M") {
            return "001";
        } else if (mnemonic == "D") {
            return "010";
        } else if (mnemonic == "MD") {
            return "011";
        } else if (mnemonic == "A") {
            return "100";
        } else if (mnemonic == "AM") {
            return "101";
        } else if (mnemonic == "AD") {
            return "110";
        } else if (mnemonic == "AMD") {
            return "111";
        }
        return "";
    }

    string comp(string mnemonic) {
        if (mnemonic == "0") {
            return "0101010";
        } else if (mnemonic == "1") {
            return "0111111";
        } else if (mnemonic == "-1") {
            return "0111010";
        } else if (mnemonic == "D") {
            return "0001100";
        } else if (mnemonic == "A") {
            return "0110000";
        } else if (mnemonic == "!D") {
            return "0001101";
        } else if (mnemonic == "!A") {
            return "0110001";
        } else if (mnemonic == "-D") {
            return "0001111";
        } else if (mnemonic == "-A") {
            return "0110011";
        } else if (mnemonic == "D+1") {
            return "0011111";
        } else if (mnemonic == "A+1") {
            return "0110111";
        } else if (mnemonic == "D-1") {
            return "0001110";
        } else if (mnemonic == "A-1") {
            return "0110010";
        } else if (mnemonic == "D+A") {
            return "0000010";
        } else if (mnemonic == "D-A") {
            return "0010011";
        } else if (mnemonic == "A-D") {
            return "0000111";
        } else if (mnemonic == "D&A") {
            return "0000000";
        } else if (mnemonic == "D|A") {
            return "0010101";
        } else if (mnemonic == "M") {
            return "1110000";
        } else if (mnemonic == "!M") {
            return "1110001";
        } else if (mnemonic == "-M") {
            return "1110011";
        } else if (mnemonic == "M+1") {
            return "1110111";
        } else if (mnemonic == "M-1") {
            return "1110010";
        } else if (mnemonic == "D+M") {
            return "1000010";
        } else if (mnemonic == "D-M") {
            return "1010011";
        } else if (mnemonic == "M-D") {
            return "1000111";
        } else if (mnemonic == "D&M") {
            return "1000000";
        } else if (mnemonic == "D|M") {
            return "1010101";
        }
        return "";
    }

    string jump(string mnemonic) {
        if (mnemonic == "") {
            return "000";
        } else if (mnemonic == "JGT") {
            return "001";
        } else if (mnemonic == "JEQ") {
            return "010";
        } else if (mnemonic == "JGE") {
            return "011";
        } else if (mnemonic == "JLT") {
            return "100";
        } else if (mnemonic == "JNE") {
            return "101";
        } else if (mnemonic == "JLE") {
            return "110";
        } else if (mnemonic == "JMP") {
            return "111";
        }
        return "";
    }
};

int main(int argc, char *argv[]) {
    string filename = argv[1];
    // string filename = "Add.asm";
    string outputFilename = filename.substr(0, filename.find('.')) + ".hack";
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
