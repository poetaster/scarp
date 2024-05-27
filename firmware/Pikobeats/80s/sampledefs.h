// sample structure built by wav2header based on wav2sketch by Paul Stoffregen

struct sample_t {
  const int16_t * samplearray; // pointer to sample array
  uint32_t samplesize; // size of the sample array
  uint32_t sampleindex; // current sample array index when playing. index at last sample= not playing
  uint8_t MIDINOTE;  // MIDI note on that plays this sample
  uint8_t play_volume; // play volume 0-127
  char sname[20];        // sample name
} sample[] = {

	Bd0000,	// pointer to sample array
	Bd0000_SIZE,	// size of the sample array
	Bd0000_SIZE,	//sampleindex. if at end of sample array sound is not playing
	35,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Bd0000",	// sample name

	Cb,	// pointer to sample array
	Cb_SIZE,	// size of the sample array
	Cb_SIZE,	//sampleindex. if at end of sample array sound is not playing
	36,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Cb",	// sample name

	Ch,	// pointer to sample array
	Ch_SIZE,	// size of the sample array
	Ch_SIZE,	//sampleindex. if at end of sample array sound is not playing
	37,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ch",	// sample name

	Cl,	// pointer to sample array
	Cl_SIZE,	// size of the sample array
	Cl_SIZE,	//sampleindex. if at end of sample array sound is not playing
	38,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Cl",	// sample name

	Cp,	// pointer to sample array
	Cp_SIZE,	// size of the sample array
	Cp_SIZE,	//sampleindex. if at end of sample array sound is not playing
	39,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Cp",	// sample name

	Cy0000,	// pointer to sample array
	Cy0000_SIZE,	// size of the sample array
	Cy0000_SIZE,	//sampleindex. if at end of sample array sound is not playing
	40,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Cy0000",	// sample name

	Cy0025,	// pointer to sample array
	Cy0025_SIZE,	// size of the sample array
	Cy0025_SIZE,	//sampleindex. if at end of sample array sound is not playing
	41,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Cy0025",	// sample name

	Hc00,	// pointer to sample array
	Hc00_SIZE,	// size of the sample array
	Hc00_SIZE,	//sampleindex. if at end of sample array sound is not playing
	42,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Hc00",	// sample name

	Lt00,	// pointer to sample array
	Lt00_SIZE,	// size of the sample array
	Lt00_SIZE,	//sampleindex. if at end of sample array sound is not playing
	43,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Lt00",	// sample name

	Ma,	// pointer to sample array
	Ma_SIZE,	// size of the sample array
	Ma_SIZE,	//sampleindex. if at end of sample array sound is not playing
	44,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ma",	// sample name

	Mt40clave,	// pointer to sample array
	Mt40clave_SIZE,	// size of the sample array
	Mt40clave_SIZE,	//sampleindex. if at end of sample array sound is not playing
	45,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Mt40clave",	// sample name

	Mt40clhat,	// pointer to sample array
	Mt40clhat_SIZE,	// size of the sample array
	Mt40clhat_SIZE,	//sampleindex. if at end of sample array sound is not playing
	46,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Mt40clhat",	// sample name

	Mt40ophat01,	// pointer to sample array
	Mt40ophat01_SIZE,	// size of the sample array
	Mt40ophat01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	47,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Mt40ophat01",	// sample name

	Oh00,	// pointer to sample array
	Oh00_SIZE,	// size of the sample array
	Oh00_SIZE,	//sampleindex. if at end of sample array sound is not playing
	48,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Oh00",	// sample name

	Rs,	// pointer to sample array
	Rs_SIZE,	// size of the sample array
	Rs_SIZE,	//sampleindex. if at end of sample array sound is not playing
	49,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Rs",	// sample name

	Sd0000,	// pointer to sample array
	Sd0000_SIZE,	// size of the sample array
	Sd0000_SIZE,	//sampleindex. if at end of sample array sound is not playing
	50,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sd0000",	// sample name

	Sd0025,	// pointer to sample array
	Sd0025_SIZE,	// size of the sample array
	Sd0025_SIZE,	//sampleindex. if at end of sample array sound is not playing
	51,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sd0025",	// sample name

	Sd7525,	// pointer to sample array
	Sd7525_SIZE,	// size of the sample array
	Sd7525_SIZE,	//sampleindex. if at end of sample array sound is not playing
	52,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sd7525",	// sample name

	Sd7550,	// pointer to sample array
	Sd7550_SIZE,	// size of the sample array
	Sd7550_SIZE,	//sampleindex. if at end of sample array sound is not playing
	53,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sd7550",	// sample name

	Sr88clhat,	// pointer to sample array
	Sr88clhat_SIZE,	// size of the sample array
	Sr88clhat_SIZE,	//sampleindex. if at end of sample array sound is not playing
	54,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sr88clhat",	// sample name

};
