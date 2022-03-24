# wav-file

Basic RIFF WAVE file writer and header reader.

`write.cpp` creates a wav file containing a sine wave with specified properties. Originally based on video from [Akash Murthy (Thriflegange)](https://github.com/Thrifleganger). Expanded to include variable bit depth, variable channel count, and stereo noise.

`read.cpp` parses a wav file header and validates basic confirmation to the format spec.

Documentation for the WAVE format can be found here: https://ccrma.stanford.edu/courses/422-winter-2014/projects/WaveFormat/
