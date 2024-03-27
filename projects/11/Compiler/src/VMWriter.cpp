#include "VMWriter.h"

VMWriter::VMWriter(const std::string& outputFilePath)
    : outputFile(outputFilePath) {}

void VMWriter::writePush(Segment segment, int index) {
    switch(segment){
        case Segment::CONST:
            outputFile << "push constant " << index << std::endl;
            break;
        case Segment::ARG:
            outputFile << "push argument " << index << std::endl;
            break;
        case Segment::LOCAL:
            outputFile << "push local " << index << std::endl;
            break;
        case Segment::STATIC:
            outputFile << "push static " << index << std::endl;
            break;
        case Segment::THIS:
            outputFile << "push this " << index << std::endl;
            break;
        case Segment::THAT:
            outputFile << "push that " << index << std::endl;
            break;
        case Segment::POINTER:
            outputFile << "push pointer " << index << std::endl;
            break;
        case Segment::TEMP:
            outputFile << "push temp " << index << std::endl;
            break;
    
    }
}

void VMWriter::writePush(Kind kind, int index) {
    switch(kind){
        case Kind::STATIC:
            outputFile << "push static " << index << std::endl;
            break;
        case Kind::FIELD:
            outputFile << "push this " << index << std::endl;
            break;
        case Kind::ARG:
            outputFile << "push argument " << index << std::endl;
            break;
        case Kind::VAR:
            outputFile << "push local " << index << std::endl;
            break;
        case Kind::NONE:
            break;
    }
}

void VMWriter::writePop(Segment segment, int index) {
    switch(segment){
        case Segment::CONST:
            outputFile << "pop constant " << index << std::endl;
            break;
        case Segment::ARG:
            outputFile << "pop argument " << index << std::endl;
            break;
        case Segment::LOCAL:
            outputFile << "pop local " << index << std::endl;
            break;
        case Segment::STATIC:
            outputFile << "pop static " << index << std::endl;
            break;
        case Segment::THIS:
            outputFile << "pop this " << index << std::endl;
            break;
        case Segment::THAT:
            outputFile << "pop that " << index << std::endl;
            break;
        case Segment::POINTER:
            outputFile << "pop pointer " << index << std::endl;
            break;
        case Segment::TEMP:
            outputFile << "pop temp " << index << std::endl;
            break;
    
    }
}

void VMWriter::writePop(Kind kind, int index) {
    switch(kind){
        case Kind::STATIC:
            outputFile << "pop static " << index << std::endl;
            break;
        case Kind::FIELD:
            outputFile << "pop this " << index << std::endl;
            break;
        case Kind::ARG:
            outputFile << "pop argument " << index << std::endl;
            break;
        case Kind::VAR:
            outputFile << "pop local " << index << std::endl;
            break;
        case Kind::NONE:
            break;
    }
}

void VMWriter::writeArithmetic(Command command) {
    switch (command) {
        case Command::ADD:
            outputFile << "add" << std::endl;
            break;
        case Command::SUB:
            outputFile << "sub" << std::endl;
            break;
        case Command::NEG:
            outputFile << "neg" << std::endl;
            break;
        case Command::EQ:
            outputFile << "eq" << std::endl;
            break;
        case Command::GT:
            outputFile << "gt" << std::endl;
            break;
        case Command::LT:
            outputFile << "lt" << std::endl;
            break;
        case Command::AND:
            outputFile << "and" << std::endl;
            break;
        case Command::OR:
            outputFile << "or" << std::endl;
            break;
        case Command::NOT:
            outputFile << "not" << std::endl;
            break;
    
    }
}

void VMWriter::writeLabel(const std::string& label) {
    outputFile << "label " << label << std::endl;
}

void VMWriter::writeGoto(const std::string& label) {
    outputFile << "goto " << label << std::endl;
}

void VMWriter::writeIf(const std::string& label) {
    outputFile << "if-goto " << label << std::endl;
}

void VMWriter::writeCall(const std::string& name, int nArgs) {
    outputFile << "call " << name << " " << nArgs << std::endl;
}

void VMWriter::writeFunction(const std::string& name, int nLocals) {
    outputFile << "function " << name << " " << nLocals << std::endl;
}

void VMWriter::writeReturn() {
    outputFile << "return" << std::endl;
}

VMWriter::~VMWriter() {
    outputFile.close();
}
