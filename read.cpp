#include <iostream>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <cstddef>
#include <cstring>

// TODO validate RIFF, WAVE, chunk sizes
void validateChunkID(const char* str1, const char* str2){
    if (strcmp(str1, str2)){
        std::cout << "Error: expected " << str1 << " but found " << str2 << std::endl;
        exit(1);
    }
    return;
}

class WavHeader {
public:
    WavHeader() {};

    // TODO parse header into separate member variables
};

int bytesToInt(std::ifstream& file, int size) {
    int initialSize = size;
    char buffer[initialSize];
    while(size > 0) {
        file.read((char*)&buffer[initialSize-size], 1);
        --size;
    }
    int value = 0;
    size = initialSize - 1;
    while(size >= 0) {
        int byte = (unsigned char)(buffer[size]) << (size * 8);
        value += byte;
        --size;
    }
    return (int)value;
}

int main() {
    WavHeader wavHeader;

    std::ifstream inputFile;
    inputFile.open("output.wav", std::ios::binary);
    std::cout << bytesToInt(inputFile, 4) << std::endl;
    std::cout << bytesToInt(inputFile, 4) << std::endl;
    std::cout << bytesToInt(inputFile, 4) << std::endl;
    std::cout << bytesToInt(inputFile, 4) << std::endl;
    std::cout << bytesToInt(inputFile, 4) << std::endl;
    std::cout << bytesToInt(inputFile, 2) << std::endl;
    std::cout << "Channels: " << bytesToInt(inputFile, 2) << std::endl;
    std::cout << "Sample Rate: " << bytesToInt(inputFile, 4) << std::endl;
    std::cout << bytesToInt(inputFile, 4) << std::endl;
    std::cout << bytesToInt(inputFile, 2) << std::endl;
    std::cout << "Bit Depth: " << bytesToInt(inputFile, 2) << std::endl;

    inputFile.close();
    return 0;
}