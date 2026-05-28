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

	A1ch,	// pointer to sample array
	A1ch_SIZE,	// size of the sample array
	A1ch_SIZE,	//sampleindex. if at end of sample array sound is not playing
	36,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"A1ch",	// sample name

	A1cr,	// pointer to sample array
	A1cr_SIZE,	// size of the sample array
	A1cr_SIZE,	//sampleindex. if at end of sample array sound is not playing
	37,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"A1cr",	// sample name

	A1oh,	// pointer to sample array
	A1oh_SIZE,	// size of the sample array
	A1oh_SIZE,	//sampleindex. if at end of sample array sound is not playing
	38,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"A1oh",	// sample name

	A1rd,	// pointer to sample array
	A1rd_SIZE,	// size of the sample array
	A1rd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	39,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"A1rd",	// sample name

	A1rs,	// pointer to sample array
	A1rs_SIZE,	// size of the sample array
	A1rs_SIZE,	//sampleindex. if at end of sample array sound is not playing
	40,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"A1rs",	// sample name

	A1sd,	// pointer to sample array
	A1sd_SIZE,	// size of the sample array
	A1sd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	41,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"A1sd",	// sample name

	A1_cl,	// pointer to sample array
	A1_cl_SIZE,	// size of the sample array
	A1_cl_SIZE,	//sampleindex. if at end of sample array sound is not playing
	42,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"A1_cl",	// sample name

	B2bd,	// pointer to sample array
	B2bd_SIZE,	// size of the sample array
	B2bd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	43,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2bd",	// sample name

	B2ch,	// pointer to sample array
	B2ch_SIZE,	// size of the sample array
	B2ch_SIZE,	//sampleindex. if at end of sample array sound is not playing
	44,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2ch",	// sample name

	B2cr,	// pointer to sample array
	B2cr_SIZE,	// size of the sample array
	B2cr_SIZE,	//sampleindex. if at end of sample array sound is not playing
	45,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2cr",	// sample name

	B2oh,	// pointer to sample array
	B2oh_SIZE,	// size of the sample array
	B2oh_SIZE,	//sampleindex. if at end of sample array sound is not playing
	46,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2oh",	// sample name

	B2rd,	// pointer to sample array
	B2rd_SIZE,	// size of the sample array
	B2rd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	47,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2rd",	// sample name

	B2rs,	// pointer to sample array
	B2rs_SIZE,	// size of the sample array
	B2rs_SIZE,	//sampleindex. if at end of sample array sound is not playing
	48,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2rs",	// sample name

	B2sd,	// pointer to sample array
	B2sd_SIZE,	// size of the sample array
	B2sd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	49,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2sd",	// sample name

	B2_cl,	// pointer to sample array
	B2_cl_SIZE,	// size of the sample array
	B2_cl_SIZE,	//sampleindex. if at end of sample array sound is not playing
	50,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2_cl",	// sample name

	C3bd,	// pointer to sample array
	C3bd_SIZE,	// size of the sample array
	C3bd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	51,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3bd",	// sample name

	C3ch,	// pointer to sample array
	C3ch_SIZE,	// size of the sample array
	C3ch_SIZE,	//sampleindex. if at end of sample array sound is not playing
	52,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3ch",	// sample name

	C3cr,	// pointer to sample array
	C3cr_SIZE,	// size of the sample array
	C3cr_SIZE,	//sampleindex. if at end of sample array sound is not playing
	53,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3cr",	// sample name

	C3oh,	// pointer to sample array
	C3oh_SIZE,	// size of the sample array
	C3oh_SIZE,	//sampleindex. if at end of sample array sound is not playing
	54,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3oh",	// sample name

	C3rd,	// pointer to sample array
	C3rd_SIZE,	// size of the sample array
	C3rd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	55,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3rd",	// sample name

	C3rs,	// pointer to sample array
	C3rs_SIZE,	// size of the sample array
	C3rs_SIZE,	//sampleindex. if at end of sample array sound is not playing
	56,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3rs",	// sample name

	C3sd,	// pointer to sample array
	C3sd_SIZE,	// size of the sample array
	C3sd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	57,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3sd",	// sample name

	C3_cl,	// pointer to sample array
	C3_cl_SIZE,	// size of the sample array
	C3_cl_SIZE,	//sampleindex. if at end of sample array sound is not playing
	58,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3_cl",	// sample name

	D4bd,	// pointer to sample array
	D4bd_SIZE,	// size of the sample array
	D4bd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	59,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4bd",	// sample name

	D4ch,	// pointer to sample array
	D4ch_SIZE,	// size of the sample array
	D4ch_SIZE,	//sampleindex. if at end of sample array sound is not playing
	60,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4ch",	// sample name

	D4cr,	// pointer to sample array
	D4cr_SIZE,	// size of the sample array
	D4cr_SIZE,	//sampleindex. if at end of sample array sound is not playing
	61,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4cr",	// sample name

	D4oh,	// pointer to sample array
	D4oh_SIZE,	// size of the sample array
	D4oh_SIZE,	//sampleindex. if at end of sample array sound is not playing
	62,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4oh",	// sample name

	D4rd,	// pointer to sample array
	D4rd_SIZE,	// size of the sample array
	D4rd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	63,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4rd",	// sample name

	D4rs,	// pointer to sample array
	D4rs_SIZE,	// size of the sample array
	D4rs_SIZE,	//sampleindex. if at end of sample array sound is not playing
	64,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4rs",	// sample name

	D4sd,	// pointer to sample array
	D4sd_SIZE,	// size of the sample array
	D4sd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	65,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4sd",	// sample name

	D4_cl,	// pointer to sample array
	D4_cl_SIZE,	// size of the sample array
	D4_cl_SIZE,	//sampleindex. if at end of sample array sound is not playing
	66,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4_cl",	// sample name

};
