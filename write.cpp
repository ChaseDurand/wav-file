#include <iostream>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cstddef>
#include <cstring>

const int sampleRate = 44100;
const int bitDepth = 16;
const int channels = 2;
const int duration = 2; // Duration in seconds

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

class Buffer {
public:
    Buffer() {}

    // Push char* to buffer byte by byte.
    void add(const char* val) {
        int length = strlen(val);
        for(int i = 0; i < length; ++i) {
            buf.push_back(val[i]);
        }
        return;
    }

    // Convert int val to size number of bytes and push to buffer.
    // Optional argument for replacing starting at position (no bounds checking).
    void add(int val, int size, int position = -1) {
        int initialSize = size;
        while(size > 0) {
            char charVal = (val >> (initialSize - (size))*8) & 0xff;
            if(position == -1) {
                buf.push_back(charVal);
            }
            else {
                buf[position+(initialSize-size)] = charVal;
            }
            size--;
        }
        return;
    }

    int size(){
        return buf.size();
    }

    auto begin() {
        return buf.begin();
    }

    auto end() {
        return buf.end();
    }
private:
    std::vector<char> buf;
};

int main() {
    srand (static_cast <unsigned> (time(0)));
    SineOscillator sineOscillator(440, 0.5);
    Buffer buffer;

    // Header chunk
    buffer.add("RIFF");
    int totalSizePos = buffer.size(); // Note current position to later fill in size
    buffer.add("----"); // Placeholder for file size (minus 8 byte header)
    buffer.add("WAVE"); // Format

    // Format chunk
    buffer.add("fmt "); // Chunk ID
    buffer.add(16, 4); // Chunk data size (16)
    buffer.add(1, 2); // Compression Code (1 == PCM/uncompressed)
    buffer.add(channels, 2); // Number of channels
    buffer.add(sampleRate, 4); // Sample rate
    buffer.add(sampleRate * bitDepth * channels / 8, 4); // Average bytes per second
    buffer.add(channels * bitDepth / 8, 2); // Block align
    buffer.add(bitDepth, 2); // Significant bits per sample
    
    // Data chunk
    buffer.add("data"); // Chunk ID
    int dataChunkSizePos = buffer.size(); // Note current position to later fill in size
    buffer.add("----"); // Placeholder for data size

    int preDataSize = buffer.size(); // Note current size to later determine total data bytes

    auto maxAmp = pow(2, bitDepth - 1) - 1; // Scale sine wave from -1:+1 based on bit depth
    for (int i = 0; i < sampleRate * duration; ++i) {
        auto sample = sineOscillator.process();
        float noiseAmp = 0.00f; // Stereo noise level
        for(int j = 0; j < channels; ++j) {
            float noise = -noiseAmp + static_cast<float>( rand() ) /
                ( static_cast<float>( RAND_MAX / (2 * noiseAmp) ) );
            float sampleTmp = sample + noise;
            sampleTmp *= maxAmp;
            buffer.add(static_cast<int>(sampleTmp), 2);
        }
    }

    // Add chunk data size (total size - 8 byte header)
    buffer.add(buffer.size() - 8, 4, totalSizePos);

    // Add format chunk data size
    buffer.add(buffer.size() - preDataSize, 4, dataChunkSizePos);

    std::ofstream fout("output.wav");
    for(auto const& x : buffer) {
        fout << x;
    }

    return 0;
}