#include "compilationEngine.h"
#include "tokenizer.h"
#include "symbolTable.h"
#include "vmWriter.h"
#include "define.h"
#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>

using std::string;
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./Compiler <input file> | <input dir>" << std::endl;
        return 1;
    }

    string inputPath = argv[1];
    // string inputPath = "../../Pong";
    std::filesystem::path path(inputPath);
    if (std::filesystem::is_directory(path)) {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.path().extension() == ".jack") {
                string inputFilePath = entry.path().string();
                string outputFilePath = inputFilePath.substr(0, inputFilePath.find_last_of('.')) + ".vm";
                CompilationEngine compiler(inputFilePath, outputFilePath);
                compiler.CompileClass();
            }
        }
    } else {
        string outputFilePath = inputPath.substr(0, inputPath.find_last_of('.')) + ".vm";
        CompilationEngine compiler(inputPath, outputFilePath);
        compiler.CompileClass();
    }

    return 0;
}