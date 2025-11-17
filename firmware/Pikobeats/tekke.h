
#define NUM_VOICES 32

struct voice_t {
  int16_t sample;   // index of the sample structure in sampledefs.h
  int16_t level;   // 0-1000 for legacy reasons
  uint32_t sampleindex; // 20:12 fixed point index into the sample array
  uint16_t sampleincrement; // 1:12 fixed point sample step for pitch changes
  bool isPlaying;  // true when sample is playing
} voice[NUM_VOICES] = {
  0,700, 0, 4096, false, //snr 10
  1,700, 0, 4096, false, //snr 10
  2,700, 0, 4096, false, //hfht
  3,700, 0, 4096, false, //ohat 3
  4,700, 0, 4096, false, //rim
  5,700, 0, 4096, false, //sdst 07
  6,700, 0, 4096, false, //tome 01
  7,700, 0, 4096, false,  //clH
  8,700, 0, 4096, false,
  9,700, 0, 4096, false,
  10,700, 0, 4096, false,
  11,700, 0, 4096, false,
  12,700, 0, 4096, false,
  13,700, 0, 4096, false,
  14,700, 0, 4096, false,
  15,700, 0, 4096, false,
  16,700, 0, 4096, false,
  17,700, 0, 4096, false,
  18,700, 0, 4096, false,
  19,700, 0, 4096, false, 
  20,700, 0, 4096, false, 
  21,700, 0, 4096, false,
  22,700, 0, 4096, false,
  23,700, 0, 4096, false,
  24,700, 0, 4096, false,
  25,700, 0, 4096, false,
  26,700, 0, 4096, false,
  27,700, 0, 4096, false,
  28,700, 0, 4096, false,
  29,700, 0, 4096, false,
  30,700, 0, 4096, false,
  31,700, 0, 4096, false,
};
#include "tekke/samples.h" // from chambord
