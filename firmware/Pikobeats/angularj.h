
#define NUM_VOICES 28

struct voice_t {
  int16_t sample;   // index of the sample structure in sampledefs.h
  int16_t level;   // 0-1000 for legacy reasons
  uint32_t sampleindex; // 20:12 fixed point index into the sample array
  uint16_t sampleincrement; // 1:12 fixed point sample step for pitch changes
  bool isPlaying;  // true when sample is playing
} voice[NUM_VOICES] = {
  0,      // default voice 0 assignment - typically a kick but you can map them any way you want
  900,  // initial level
  1,    // sampleindex
  4096, // initial pitch step - normal pitch
  false, // sample not playing
  1,900, 0, 4096, false,
  2,900, 0, 4096, false, 
  11,900, 0, 4096, false,
  7,900, 0, 4096, false,
  12,900, 0, 4096, false,
  19,900, 0, 4096, false,
  18,900, 0, 4096, false,
  8,900, 0, 4096, false,
  9,900, 0, 4096, false,
  10,900, 0, 4096, false,
  3,900, 0, 4096, false,
  5,900, 0, 4096, false,
  13,900, 0, 4096, false,
  14,900, 0, 4096, false,
  15,900, 0, 4096, false,
  16,900, 0, 4096, false,
  17,900, 0, 4096, false,
  4,900, 0, 4096, false,
  6,900, 0, 4096, false, 
  20,900, 0, 4096, false, 
  21,900, 0, 4096, false,
  22,900, 0, 4096, false,
  23,900, 0, 4096, false,
  24,900, 0, 4096, false,
  25,900, 0, 4096, false,
  26,900, 0, 4096, false,
  27,900, 0, 4096, false,
};
#include "Angular_Jungle_Set/samples.h" // 808, mt40sr88sy1, sounds
