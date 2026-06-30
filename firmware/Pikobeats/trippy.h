
#define NUM_VOICES 32

struct voice_t {
  int16_t sample;   // index of the sample structure in sampledefs.h
  int16_t level;   // 0-1000 for legacy reasons
  uint32_t sampleindex; // 20:12 fixed point index into the sample array
  uint16_t sampleincrement; // 1:12 fixed point sample step for pitch changes
  bool isPlaying;  // true when sample is playing
} voice[NUM_VOICES] = {
  0,100, 0, 4096, false,
  1,100, 0, 4096, false,
  2,100, 0, 4096, false,
  3,100, 0, 4096, false,
  4,100, 0, 4096, false,
  5,100, 0, 4096, false,
  6,100, 0, 4096, false,
  7,100, 0, 4096, false,
  8,100, 0, 4096, false,
  9,100, 0, 4096, false,
  10,100, 0, 4096, false,
  11,100, 0, 4096, false,
  12,100, 0, 4096, false,
  13,100, 0, 4096, false,
  14,100, 0, 4096, false,
  15,100, 0, 4096, false,
  16,100, 0, 4096, false,
  17,100, 0, 4096, false,
  18,100, 0, 4096, false,
  19,100, 0, 4096, false, 
  20,100, 0, 4096, false, 
  21,100, 0, 4096, false,
  22,100, 0, 4096, false,
  23,100, 0, 4096, false,
  24,100, 0, 4096, false,
  25,100, 0, 4096, false,
  26,100, 0, 4096, false,
  27,100, 0, 4096, false,
  28,100, 0, 4096, false,
  29,100, 0, 4096, false,
  30,100, 0, 4096, false,
  31,100, 0, 4096, false,
};
#include "trippy/samples.h" 
