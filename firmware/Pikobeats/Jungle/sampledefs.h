// sample structure built by wav2header based on wav2sketch by Paul Stoffregen

struct sample_t {
  const int16_t * samplearray; // pointer to sample array
  uint32_t samplesize; // size of the sample array
  uint32_t sampleindex; // current sample array index when playing. index at last sample= not playing
  uint8_t MIDINOTE;  // MIDI note on that plays this sample
  uint8_t play_volume; // play volume 0-127
  char sname[20];        // sample name
} sample[] = {

	Ws02,	// pointer to sample array
	Ws02_SIZE,	// size of the sample array
	Ws02_SIZE,	//sampleindex. if at end of sample array sound is not playing
	35,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws02",	// sample name

	Ws04,	// pointer to sample array
	Ws04_SIZE,	// size of the sample array
	Ws04_SIZE,	//sampleindex. if at end of sample array sound is not playing
	36,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws04",	// sample name

	Ws08,	// pointer to sample array
	Ws08_SIZE,	// size of the sample array
	Ws08_SIZE,	//sampleindex. if at end of sample array sound is not playing
	37,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws08",	// sample name

	Ws09,	// pointer to sample array
	Ws09_SIZE,	// size of the sample array
	Ws09_SIZE,	//sampleindex. if at end of sample array sound is not playing
	38,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws09",	// sample name

	Ws11,	// pointer to sample array
	Ws11_SIZE,	// size of the sample array
	Ws11_SIZE,	//sampleindex. if at end of sample array sound is not playing
	39,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws11",	// sample name

	Ws21,	// pointer to sample array
	Ws21_SIZE,	// size of the sample array
	Ws21_SIZE,	//sampleindex. if at end of sample array sound is not playing
	40,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws21",	// sample name

	Ws23,	// pointer to sample array
	Ws23_SIZE,	// size of the sample array
	Ws23_SIZE,	//sampleindex. if at end of sample array sound is not playing
	41,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws23",	// sample name

	Ws31,	// pointer to sample array
	Ws31_SIZE,	// size of the sample array
	Ws31_SIZE,	//sampleindex. if at end of sample array sound is not playing
	42,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws31",	// sample name

	Ws36,	// pointer to sample array
	Ws36_SIZE,	// size of the sample array
	Ws36_SIZE,	//sampleindex. if at end of sample array sound is not playing
	43,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws36",	// sample name

	Ws39,	// pointer to sample array
	Ws39_SIZE,	// size of the sample array
	Ws39_SIZE,	//sampleindex. if at end of sample array sound is not playing
	44,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws39",	// sample name

	Ws40,	// pointer to sample array
	Ws40_SIZE,	// size of the sample array
	Ws40_SIZE,	//sampleindex. if at end of sample array sound is not playing
	45,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws40",	// sample name

	Ws42,	// pointer to sample array
	Ws42_SIZE,	// size of the sample array
	Ws42_SIZE,	//sampleindex. if at end of sample array sound is not playing
	46,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws42",	// sample name

	Ws43,	// pointer to sample array
	Ws43_SIZE,	// size of the sample array
	Ws43_SIZE,	//sampleindex. if at end of sample array sound is not playing
	47,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws43",	// sample name

	Ws53,	// pointer to sample array
	Ws53_SIZE,	// size of the sample array
	Ws53_SIZE,	//sampleindex. if at end of sample array sound is not playing
	48,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws53",	// sample name

};
