#include <iostream>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <ctime>

const int sampleRate = 44100;
const int bitDepth = 16;
const int channels = 2;

class SineOscillator {


    float frequency, amplitude, angle = 0.0f, offset = 0.0;
    public:
    SineOscillator(float freq, float amp) : frequency(freq), amplitude(amp) {
        offset = 2 * M_PI * frequency / sampleRate;
    }
    float process() {
        auto sample =  amplitude * sin(angle);
        angle += offset;
        return sample;
    }

};

void writeToFile(std::ofstream &file, int value, int size) {
    file.write(reinterpret_cast<const char*>(&value), size);
}

int main() {
    srand (static_cast <unsigned> (time(0)));
    SineOscillator sineOscillator(440, 0.5);
    int duration = 2; // Duration in seconds

    std::ofstream audioFile;
    audioFile.open("waveform.wav", std::ios::binary);

    // Header chunk
    audioFile << "RIFF"; // Chunk ID
    audioFile << "----"; // Placeholder for file size-8
    audioFile << "WAVE";

    // Format chunk
    audioFile << "fmt "; // Chunk ID
    writeToFile(audioFile, 16, 4); // Chunk data size
    writeToFile(audioFile, 1, 2); // Compression Code
    writeToFile(audioFile, channels, 2); // Number of channels
    writeToFile(audioFile, sampleRate, 4); // Sample rate
    writeToFile(audioFile, sampleRate * bitDepth * channels / 8, 4); // Average bytes per second
    writeToFile(audioFile, channels * bitDepth / 8, 2); // Block align
    writeToFile(audioFile, bitDepth, 2); // Significant bits per sample

    // Data chunk
    audioFile << "data"; // Chunk ID
    audioFile << "----"; // Size

    int preAudioPosition = audioFile.tellp(); // Get current pointer position in stream

    auto maxAmp = pow(2, bitDepth - 1) - 1;
    for (int i = 0; i < sampleRate * duration; ++i) {
        auto sample = sineOscillator.process();
        float noiseAmp = 0.00f; // Stereo noise level
        for(int j = 0; j < channels; ++j) {
            float noise = -noiseAmp + static_cast<float>( rand() ) /
                ( static_cast<float>( RAND_MAX / (2 * noiseAmp) ) );
            sample += noise;
            int intSample = static_cast<int>(sample * maxAmp);
            writeToFile(audioFile, intSample, bitDepth / 8);
        }
    }

    int postAudioPosition = audioFile.tellp();

    audioFile.seekp(preAudioPosition - 4);
    writeToFile(audioFile, postAudioPosition - preAudioPosition, 4); // Data size (audio size)

    audioFile.seekp(4, std::ios::beg);
    writeToFile(audioFile, postAudioPosition - 8, 4); // Size in header (everything minus header)

    audioFile.close();
    return 0;
}