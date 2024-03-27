#ifndef VMWRITER_H
#define VMWRITER_H

#include <fstream>
#include <string>
#include "define.h"

class VMWriter {
public:
    VMWriter(const std::string& outputFilePath);
    void writePush(Segment segment, int index);
    void writePush(Kind kind, int index);
    void writePop(Segment segment, int index);
    void writePop(Kind kind, int index);
    void writeArithmetic(Command command);
    void writeLabel(const std::string& label);
    void writeGoto(const std::string& label);
    void writeIf(const std::string& label);
    void writeCall(const std::string& name, int nArgs);
    void writeFunction(const std::string& name, int nLocals);
    void writeReturn();
    ~VMWriter();
private:
    std::ofstream outputFile;
};

#endif