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
    // RIFF chunk descriptor
    std::string RIFF;
    int totalSize;
    std::string WAVE;
    // fmt sub chunk
    std::string fmt;
    int fmtSize;
    int compressionCode;
    int channels;
    int sampleRate;
    int bytesPerSec;
    int blockAlign;
    int bitDepth;
    // data sub chunk
    std::string data;
    int dataSize;

    // Get header info
    void getHeader(std::ifstream& file) {
        RIFF = byteToStr(file);
        totalSize = bytesToInt(file, 4);
        WAVE = byteToStr(file);
        fmt = byteToStr(file);
        fmtSize =  bytesToInt(file, 4);
        compressionCode = bytesToInt(file, 2);
        channels = bytesToInt(file, 2);
        sampleRate = bytesToInt(file, 4);
        bytesPerSec = bytesToInt(file, 4);
        blockAlign = bytesToInt(file, 2);
        bitDepth = bytesToInt(file, 2);
        data = byteToStr(file);
        dataSize = bytesToInt(file, 4);
        return;
    }

    // Print header
    void info() {
        // RIFF chunk
        std::cout << "RIFF header: " << RIFF << std::endl;
        std::cout << "Header chunk size: " << totalSize << std::endl;
        std::cout << "WAVE header: " << WAVE << std::endl;
        // fmt chunk
        std::cout << "FMT header: " << fmt << std::endl;
        std::cout << "FMT chunk size: " << fmtSize << std::endl;
        std::cout << "Compression Code: " << compressionCode << std::endl;
        std::cout << "Channels: " << channels << std::endl;
        std::cout << "Sample rate: " << sampleRate << std::endl;
        std::cout << "Bytes per second: " << bytesPerSec << std::endl;
        std::cout << "Block align: " << blockAlign << std::endl;
        std::cout << "Bit depth: " << bitDepth << std::endl;
        // Data chunk
        std::cout << "Data header: " << data << std::endl;
        std::cout << "Data chunk size: " << dataSize << std::endl;
    }

private:
    // Convert a given number of little-endian bytes to a single int
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

    // Convert a given number of bytes to a string
    std::string byteToStr(std::ifstream& file, int size=4) {
        std::string result;
        char character;
        while (size > 0) {
            file.read(&character, 1);
            result.append(&character);
            --size;
        }
        return result;
    }
};

int main() {
    WavHeader wavHeader;
    std::ifstream inputFile;
    inputFile.open("output.wav", std::ios::binary);
    wavHeader.getHeader(inputFile);
    wavHeader.info();
    inputFile.close();
    return 0;
}