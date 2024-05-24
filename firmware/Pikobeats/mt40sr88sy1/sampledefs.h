// sample structure built by wav2header based on wav2sketch by Paul Stoffregen

struct sample_t {
  const int16_t * samplearray; // pointer to sample array
  uint32_t samplesize; // size of the sample array
  uint32_t sampleindex; // current sample array index when playing. index at last sample= not playing
  uint8_t MIDINOTE;  // MIDI note on that plays this sample
  uint8_t play_volume; // play volume 0-127
  char sname[20];        // sample name
} sample[] = {

	Mt40clave,	// pointer to sample array
	Mt40clave_SIZE,	// size of the sample array
	Mt40clave_SIZE,	//sampleindex. if at end of sample array sound is not playing
	35,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Mt40clave",	// sample name

	Mt40clhat,	// pointer to sample array
	Mt40clhat_SIZE,	// size of the sample array
	Mt40clhat_SIZE,	//sampleindex. if at end of sample array sound is not playing
	36,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Mt40clhat",	// sample name


	Mt40ophat01,	// pointer to sample array
	Mt40ophat01_SIZE,	// size of the sample array
	Mt40ophat01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	39,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Mt40ophat01",	// sample name

	Mt40snr01,	// pointer to sample array
	Mt40snr01_SIZE,	// size of the sample array
	Mt40snr01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	41,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Mt40snr01",	// sample name

	Sr88clhat,	// pointer to sample array
	Sr88clhat_SIZE,	// size of the sample array
	Sr88clhat_SIZE,	//sampleindex. if at end of sample array sound is not playing
	42,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sr88clhat",	// sample name

	Sr88kick,	// pointer to sample array
	Sr88kick_SIZE,	// size of the sample array
	Sr88kick_SIZE,	//sampleindex. if at end of sample array sound is not playing
	43,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sr88kick",	// sample name

	Sr88ophat,	// pointer to sample array
	Sr88ophat_SIZE,	// size of the sample array
	Sr88ophat_SIZE,	//sampleindex. if at end of sample array sound is not playing
	44,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sr88ophat",	// sample name

	Sr88snr,	// pointer to sample array
	Sr88snr_SIZE,	// size of the sample array
	Sr88snr_SIZE,	//sampleindex. if at end of sample array sound is not playing
	45,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sr88snr",	// sample name

	Sy1clhat,	// pointer to sample array
	Sy1clhat_SIZE,	// size of the sample array
	Sy1clhat_SIZE,	//sampleindex. if at end of sample array sound is not playing
	46,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sy1clhat",	// sample name

	Sy1hfhat,	// pointer to sample array
	Sy1hfhat_SIZE,	// size of the sample array
	Sy1hfhat_SIZE,	//sampleindex. if at end of sample array sound is not playing
	47,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sy1hfhat",	// sample name

	Sy1perc01,	// pointer to sample array
	Sy1perc01_SIZE,	// size of the sample array
	Sy1perc01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	49,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sy1perc01",	// sample name

	Sy1perc02,	// pointer to sample array
	Sy1perc02_SIZE,	// size of the sample array
	Sy1perc02_SIZE,	//sampleindex. if at end of sample array sound is not playing
	50,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sy1perc02",	// sample name

	Sy1perc03,	// pointer to sample array
	Sy1perc03_SIZE,	// size of the sample array
	Sy1perc03_SIZE,	//sampleindex. if at end of sample array sound is not playing
	51,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sy1perc03",	// sample name

	Sy1perc04,	// pointer to sample array
	Sy1perc04_SIZE,	// size of the sample array
	Sy1perc04_SIZE,	//sampleindex. if at end of sample array sound is not playing
	52,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sy1perc04",	// sample name

	Sy1snr02,	// pointer to sample array
	Sy1snr02_SIZE,	// size of the sample array
	Sy1snr02_SIZE,	//sampleindex. if at end of sample array sound is not playing
	53,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sy1snr02",	// sample name

	Sy1snr03,	// pointer to sample array
	Sy1snr03_SIZE,	// size of the sample array
	Sy1snr03_SIZE,	//sampleindex. if at end of sample array sound is not playing
	54,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sy1snr03",	// sample name

	Sy1tom01,	// pointer to sample array
	Sy1tom01_SIZE,	// size of the sample array
	Sy1tom01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	55,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sy1tom01",	// sample name

};
