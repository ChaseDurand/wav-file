#include <iostream>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cstddef>

const int sampleRate = 44100;
const int bitDepth = 16;
const int channels = 2;

class SineOscillator {
public:
    SineOscillator(float freq, float amp) : frequency(freq), amplitude(amp) {
        offset = 2 * M_PI * frequency / sampleRate;
    }
    float process() {
        auto sample =  amplitude * sin(angle);
        angle += offset;
        return sample;
    }
private:
    float frequency, amplitude, angle = 0.0f, offset = 0.0;
};

void addNumber(std::vector<char>& buf, int pos, int val) {
    char a = (val >> 24) & 0xff;  // high-order (leftmost) byte: bits 24-31
    char b = (val >> 16) & 0xff;  // next byte, counting from left: bits 16-23
    char c = (val >>  8) & 0xff;  // next byte, bits 8-15
    char d = val         & 0xff;  // low-order byte: bits 0-7

    buf[pos] = d;
    buf[pos+1] = c;
    buf[pos+2] = b;
    buf[pos+3] = a;

    return;
}


// void writeToFile(std::ofstream &file, int value, int size) {
//     file.write(reinterpret_cast<const char*>(&value), size);
// }

// void appendToBuffer(std::vector<char>& buffer, const char* value) {
//     for(auto iter = value.begin(); iter != value.end(); ++iter) {
//         buffer.push_back(reinterpret_cast<const char*>(&value));
//     }
// }

// std::vector<char> intToByteArray(int value, int size) {
//     std::vector<char> result[size];
//     for(int i = 0; i < size; ++i) {
//         result[(size-1)-i] = static_cast<char> (value & 0x000000ff);
//         value = value >> 8;
//     }
//     return result;
// }

int main() {
    srand (static_cast <unsigned> (time(0)));
    SineOscillator sineOscillator(440, 0.5);
    int duration = 2; // Duration in seconds

    std::vector<char> buffer;


    // Header chunk
    // appendToBuffer(buffer, 'RIFF'); // Chunk ID
    buffer.push_back('R');
    buffer.push_back('I');
    buffer.push_back('F');
    buffer.push_back('F');

    // appendToBuffer(buffer, '----'); // Placeholder for file size-8
    buffer.push_back('-');
    buffer.push_back('-');
    buffer.push_back('-');
    buffer.push_back('-');
    // appendToBuffer(buffer, 'WAVE');
    buffer.push_back('W');
    buffer.push_back('A');
    buffer.push_back('V');
    buffer.push_back('E');

    // Format chunk
    // appendToBuffer(buffer, 'fmt '); // Chunk ID
    buffer.push_back('f');
    buffer.push_back('m');
    buffer.push_back('t');
    buffer.push_back(' ');

    // appendToBuffer(buffer, 16); // Chunk data size (16)
    buffer.push_back(16);
    buffer.push_back(0);
    buffer.push_back(0);
    buffer.push_back(0);
    

    // appendToBuffer(buffer, 1); // Compression Code (1 == PCM/uncompressed)
    buffer.push_back(1);
    buffer.push_back(0);
    
    
    // appendToBuffer(buffer, channels); // Number of channels
    buffer.push_back(channels);
    buffer.push_back(0);
    

    // appendToBuffer(buffer, sampleRate, 4); // Sample rate
    char sRa = (sampleRate) & 0xff;
    char sRb = (sampleRate >>  8) & 0xff;
    buffer.push_back(sRa);
    buffer.push_back(sRb);
    buffer.push_back(0);
    buffer.push_back(0);
    

    // appendToBuffer(buffer, ); // Average bytes per second

    char aBpSa = (sampleRate * bitDepth * channels / 8) & 0xff;
    buffer.push_back(aBpSa);
    aBpSa = ((sampleRate * bitDepth * channels / 8) >> 8) & 0xff;
    buffer.push_back(aBpSa);
    aBpSa = ((sampleRate * bitDepth * channels / 8) >> 16) & 0xff;
    buffer.push_back(aBpSa);
    aBpSa = ((sampleRate * bitDepth * channels / 8) >>24) & 0xff;
    buffer.push_back(aBpSa);
    

    // appendToBuffer(buffer, channels * bitDepth / 8); // Block align
    buffer.push_back(channels * bitDepth / 8);
    buffer.push_back(0);
    


    // appendToBuffer(buffer, bitDepth); // Significant bits per sample
    buffer.push_back(bitDepth);
    buffer.push_back(0);
    
    // Data chunk
    // appendToBuffer(buffer, 'data'); // Chunk ID
    buffer.push_back('d');
    buffer.push_back('a');
    buffer.push_back('t');
    buffer.push_back('a');


    // appendToBuffer(buffer, '----'); // Size
    buffer.push_back('-');
    buffer.push_back('-');
    buffer.push_back('-');
    buffer.push_back('-');

    // int preAudioPosition = audioFile.tellp(); // Get current pointer position in stream

    int preAudioSize = buffer.size();

    auto maxAmp = pow(2, bitDepth - 1) - 1;
    for (int i = 0; i < sampleRate * duration; ++i) {
        auto sample = sineOscillator.process();
        float noiseAmp = 0.00f; // Stereo noise level
        for(int j = 0; j < channels; ++j) {
            float noise = -noiseAmp + static_cast<float>( rand() ) /
                ( static_cast<float>( RAND_MAX / (2 * noiseAmp) ) );
            float sampleTmp = sample + noise;
            sampleTmp *= maxAmp;
            int sampleInt = static_cast<int>(sampleTmp);
            char c = (sampleInt >>  8) & 0xff;  // next byte, bits 8-15
            char d = sampleInt         & 0xff;  // low-order byte: bits 0-7
            buffer.push_back(d);
            buffer.push_back(c);
        }
    }

    int postAudioSize = buffer.size();
    int dataSize = postAudioSize - preAudioSize;
    int chunkDataSize = postAudioSize - 8;
    addNumber(buffer, 4, chunkDataSize);
    addNumber(buffer, preAudioSize-4, dataSize);

    std::ofstream fout("output.wav");
    for(auto const& x : buffer) {
        fout << x;
    }

    return 0;
}