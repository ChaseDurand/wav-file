#include <iostream>
#include <cmath>
#include <fstream>

const int sampleRate = 44100;
const int bitDepth = 16;

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

int main() {
    SineOscillator sineOscillator(440, 0.5);
    int duration = 2; // Duration in seconds

    std::ofstream audioFile;
    audioFile.open("waveform", std::ios::binary);

    auto maxAmp = pow(2, bitDepth - 1) - 1;
    for (int i = 0; i < sampleRate * duration; ++i) {
        auto sample = sineOscillator.process();
        int intSample = static_cast<int>(sample * maxAmp);
        audioFile.write(reinterpret_cast<char*>(&intSample), 2);
    }

    audioFile.close();

    return 0;
}