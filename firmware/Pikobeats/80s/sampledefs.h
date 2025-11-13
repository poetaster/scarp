// sample structure built by wav2header based on wav2sketch by Paul Stoffregen

struct sample_t {
  const int16_t * samplearray; // pointer to sample array
  uint32_t samplesize; // size of the sample array
  uint32_t sampleindex; // current sample array index when playing. index at last sample= not playing
  uint8_t MIDINOTE;  // MIDI note on that plays this sample
  uint8_t play_volume; // play volume 0-127
  char sname[20];        // sample name
} sample[] = {

	A1bd,	// pointer to sample array
	A1bd_SIZE,	// size of the sample array
	A1bd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	35,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"A1bd",	// sample name

	A1bsnare,	// pointer to sample array
	A1bsnare_SIZE,	// size of the sample array
	A1bsnare_SIZE,	//sampleindex. if at end of sample array sound is not playing
	36,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"A1bsnare",	// sample name

	A1cong,	// pointer to sample array
	A1cong_SIZE,	// size of the sample array
	A1cong_SIZE,	//sampleindex. if at end of sample array sound is not playing
	37,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"A1cong",	// sample name

	A1cym,	// pointer to sample array
	A1cym_SIZE,	// size of the sample array
	A1cym_SIZE,	//sampleindex. if at end of sample array sound is not playing
	38,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"A1cym",	// sample name

	A1hat,	// pointer to sample array
	A1hat_SIZE,	// size of the sample array
	A1hat_SIZE,	//sampleindex. if at end of sample array sound is not playing
	39,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"A1hat",	// sample name

	A1perc,	// pointer to sample array
	A1perc_SIZE,	// size of the sample array
	A1perc_SIZE,	//sampleindex. if at end of sample array sound is not playing
	40,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"A1perc",	// sample name

	A1tom,	// pointer to sample array
	A1tom_SIZE,	// size of the sample array
	A1tom_SIZE,	//sampleindex. if at end of sample array sound is not playing
	41,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"A1tom",	// sample name

	A1znd,	// pointer to sample array
	A1znd_SIZE,	// size of the sample array
	A1znd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	42,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"A1znd",	// sample name

	B2bd,	// pointer to sample array
	B2bd_SIZE,	// size of the sample array
	B2bd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	43,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2bd",	// sample name

	B2bsnare,	// pointer to sample array
	B2bsnare_SIZE,	// size of the sample array
	B2bsnare_SIZE,	//sampleindex. if at end of sample array sound is not playing
	44,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2bsnare",	// sample name

	B2cong,	// pointer to sample array
	B2cong_SIZE,	// size of the sample array
	B2cong_SIZE,	//sampleindex. if at end of sample array sound is not playing
	45,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2cong",	// sample name

	B2cym,	// pointer to sample array
	B2cym_SIZE,	// size of the sample array
	B2cym_SIZE,	//sampleindex. if at end of sample array sound is not playing
	46,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2cym",	// sample name

	B2hat,	// pointer to sample array
	B2hat_SIZE,	// size of the sample array
	B2hat_SIZE,	//sampleindex. if at end of sample array sound is not playing
	47,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2hat",	// sample name

	B2perc,	// pointer to sample array
	B2perc_SIZE,	// size of the sample array
	B2perc_SIZE,	//sampleindex. if at end of sample array sound is not playing
	48,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2perc",	// sample name

	B2tom,	// pointer to sample array
	B2tom_SIZE,	// size of the sample array
	B2tom_SIZE,	//sampleindex. if at end of sample array sound is not playing
	49,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2tom",	// sample name

	B2znd,	// pointer to sample array
	B2znd_SIZE,	// size of the sample array
	B2znd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	50,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2znd",	// sample name

	C3bd,	// pointer to sample array
	C3bd_SIZE,	// size of the sample array
	C3bd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	51,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3bd",	// sample name

	C3bsnare,	// pointer to sample array
	C3bsnare_SIZE,	// size of the sample array
	C3bsnare_SIZE,	//sampleindex. if at end of sample array sound is not playing
	52,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3bsnare",	// sample name

	C3cong,	// pointer to sample array
	C3cong_SIZE,	// size of the sample array
	C3cong_SIZE,	//sampleindex. if at end of sample array sound is not playing
	53,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3cong",	// sample name

	C3cym,	// pointer to sample array
	C3cym_SIZE,	// size of the sample array
	C3cym_SIZE,	//sampleindex. if at end of sample array sound is not playing
	54,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3cym",	// sample name

	C3hat,	// pointer to sample array
	C3hat_SIZE,	// size of the sample array
	C3hat_SIZE,	//sampleindex. if at end of sample array sound is not playing
	55,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3hat",	// sample name

	C3perc,	// pointer to sample array
	C3perc_SIZE,	// size of the sample array
	C3perc_SIZE,	//sampleindex. if at end of sample array sound is not playing
	56,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3perc",	// sample name

	C3tom,	// pointer to sample array
	C3tom_SIZE,	// size of the sample array
	C3tom_SIZE,	//sampleindex. if at end of sample array sound is not playing
	57,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3tom",	// sample name

	C3znd,	// pointer to sample array
	C3znd_SIZE,	// size of the sample array
	C3znd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	58,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3znd",	// sample name

	D4bd,	// pointer to sample array
	D4bd_SIZE,	// size of the sample array
	D4bd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	59,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4bd",	// sample name

	D4bsnare,	// pointer to sample array
	D4bsnare_SIZE,	// size of the sample array
	D4bsnare_SIZE,	//sampleindex. if at end of sample array sound is not playing
	60,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4bsnare",	// sample name

	D4cong,	// pointer to sample array
	D4cong_SIZE,	// size of the sample array
	D4cong_SIZE,	//sampleindex. if at end of sample array sound is not playing
	61,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4cong",	// sample name

	D4cym,	// pointer to sample array
	D4cym_SIZE,	// size of the sample array
	D4cym_SIZE,	//sampleindex. if at end of sample array sound is not playing
	62,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4cym",	// sample name

	D4hat,	// pointer to sample array
	D4hat_SIZE,	// size of the sample array
	D4hat_SIZE,	//sampleindex. if at end of sample array sound is not playing
	63,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4hat",	// sample name

	D4perc,	// pointer to sample array
	D4perc_SIZE,	// size of the sample array
	D4perc_SIZE,	//sampleindex. if at end of sample array sound is not playing
	64,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4perc",	// sample name

	D4tom,	// pointer to sample array
	D4tom_SIZE,	// size of the sample array
	D4tom_SIZE,	//sampleindex. if at end of sample array sound is not playing
	65,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4tom",	// sample name

	D4znd,	// pointer to sample array
	D4znd_SIZE,	// size of the sample array
	D4znd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	66,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4znd",	// sample name

};
