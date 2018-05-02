
#include <iostream>
#include <fstream>
#include <string> 


// This pragma turns off struct padding for the Dataa struct

#pragma pack (push, 1)

struct Data {
    char name[100];
    int age;
    double height;
};

// reinstate struct padding for future structs

#pragma pack (pop)



int main() {

    std::string filename = "data.bin";

    Data data = { "Pixie", 120, 0.8 };

    std::ofstream output;

    output.open(filename, std::ios::binary);

    if (!output.is_open()) {
        std::cout << "Could not create " << filename << std::endl;
    }

    output.write((char *) &data, sizeof(data));

    if (!output) {
        std::cout << "Could not write data to file " << filename << std::endl;
    }

    output.close();

    std::ifstream input;

    input.open(filename, std::ios::binary);

    if (!input.is_open()) {
        std::cout << "Could not read " << filename << std::endl;
    }

    Data inputData;

    input.read((char *) &inputData, sizeof(data));

    if (!input) {
        std::cout << "Could not read data from file " << filename << std::endl;
    }

    input.close();

    std::cout << inputData.name << ": " <<
      inputData.age << ": " << inputData.height << std::endl;

    return 0;
}
