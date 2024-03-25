#include <fstream>
#include <string>
#include <iostream>

class File { // Renamed the class to 'File'
private:
    std::ifstream file;

public:
    File(const char* filename) {
        file.open(filename);
    }
    ~File() {
        file.close();
    }
    bool is_open() {
        return file.is_open();
    }
    bool eof() {
        return file.eof();
    }
    void getline(std::string& line) {
        std::getline(file, line);
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " file1 file2" << std::endl;
        return 1;
    }

    File file1(argv[1]); // Updated the class name here
    File file2(argv[2]); // Updated the class name here

    if (!file1.is_open()) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    if (!file2.is_open()) {
        std::cerr << "Failed to open file: " << argv[2] << std::endl;
        return 1;
    }

    std::string line1;
    std::string line2;

    int line = 1;
    while (!file1.eof() && !file2.eof()){
        file1.getline(line1);
        file2.getline(line2);
        if (line1 != line2) {
            std::cout << "Files differ at line " << line << std::endl;
            return 1;
        }
        line++;
    }

    if (!file1.eof() || !file2.eof()) {
        std::cout << "Files have different number of lines" << std::endl;
        return 1;
    }

    std::cout << "Files are identical" << std::endl;
    return 0;
}