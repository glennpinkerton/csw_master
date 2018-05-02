
#include <iostream>
#include <fstream>
#include <sstream>

int main() {

    std::string inFileName = "test.txt";

    std::ifstream inFile;

    inFile.open(inFileName);


    if (inFile.is_open()) {

        std::string line;

        while (inFile) {
            std::getline(inFile, line);
      // Do not output if an error happened.  Break from the
      // loop instead.  Attempting to tread past EOF is an expected
      // "error" that will break from the loop.  This error check
      // prevents an extra empty line of output after the EOF has
      // been reached.
            if (inFile) {
                std::cout << line << std::endl;
            }
            else {
                break;
            }
        }

        inFile.close();
    } else {
        std::cout << "Cannot open file: " << inFileName << std::endl;
    }

    return 0;
}
