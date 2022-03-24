#include <iostream>
#include <fstream>
#include <limits>

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
    // Calculated chunk sizes
    int foundTotalSize;
    int fountFmtSize;
    int foundDataSize;

    // Get header info
    void getHeader(std::ifstream& file) {
        // RIFF
        RIFF = byteToStr(file);
        totalSize = bytesToInt(file, 4);
        std::streampos posMarkerTotal = file.tellg();
        WAVE = byteToStr(file);
        // fmt
        fmt = byteToStr(file);
        fmtSize =  bytesToInt(file, 4);
        std::streampos posMarkerFmt = file.tellg();
        compressionCode = bytesToInt(file, 2);
        channels = bytesToInt(file, 2);
        sampleRate = bytesToInt(file, 4);
        bytesPerSec = bytesToInt(file, 4);
        blockAlign = bytesToInt(file, 2);
        bitDepth = bytesToInt(file, 2);
        fountFmtSize = static_cast<int>(file.tellg() - posMarkerFmt);
        // data
        data = byteToStr(file);
        dataSize = bytesToInt(file, 4);
        std::streampos posMarkerData = file.tellg();
        // Seek to end to find sizes
        file.ignore( std::numeric_limits<std::streamsize>::max() );
        file.seekg(0, std::ios_base::end);
        foundTotalSize = static_cast<int>(file.tellg() - posMarkerTotal);
        foundDataSize = static_cast<int>(file.tellg() - posMarkerData);
        return;
    }
    
    // Validate header info.
    // If any are invalid, exit program.
    void validateHeader() {
        bool valid = true;
        valid &= isEqual(RIFF, std::string("RIFF"));
        valid &= isEqual(WAVE, std::string("WAVE"));
        valid &= isEqual(fmt, std::string("fmt "));
        valid &= isEqual(data, std::string("data"));
        valid &= isEqual(foundTotalSize, totalSize);
        valid &= isEqual(fountFmtSize, fmtSize);
        valid &= isEqual(foundDataSize, dataSize);
        if (!valid) {
            exit(1);
        }
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
        // Calculated info
        float length = dataSize / bytesPerSec;
        printf("Length: %.2fs\n", length);
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

    // Validate found vs expected string.
    // If mismatch, printing error and return false.
    template <class T>
    bool isEqual(const T& found, const T& expected){
        // if (found.compare(expected)){
        if (found != expected){
            std::cout << "Error: expected '" << expected << "' but found '" << found << "'" << std::endl;
            return false;
        }
        return true;
    }
};

int main() {
    WavHeader wavHeader;
    std::ifstream inputFile;
    inputFile.open("output.wav", std::ios::binary);
    wavHeader.getHeader(inputFile);
    wavHeader.validateHeader();
    wavHeader.info();
    inputFile.close();
    return 0;
}