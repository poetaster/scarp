// sample structure built by wav2header based on wav2sketch by Paul Stoffregen

struct sample_t {
  const int16_t * samplearray; // pointer to sample array
  uint32_t samplesize; // size of the sample array
  uint32_t sampleindex; // current sample array index when playing. index at last sample= not playing
  uint8_t MIDINOTE;  // MIDI note on that plays this sample
  uint8_t play_volume; // play volume 0-127
  char sname[20];        // sample name
} sample[] = {

	A1asnare,	// pointer to sample array
	A1asnare_SIZE,	// size of the sample array
	A1asnare_SIZE,	//sampleindex. if at end of sample array sound is not playing
	35,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"A1asnare",	// sample name

	A1bd,	// pointer to sample array
	A1bd_SIZE,	// size of the sample array
	A1bd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	36,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"A1bd",	// sample name

	A1clap,	// pointer to sample array
	A1clap_SIZE,	// size of the sample array
	A1clap_SIZE,	//sampleindex. if at end of sample array sound is not playing
	37,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"A1clap",	// sample name

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

	A1rim,	// pointer to sample array
	A1rim_SIZE,	// size of the sample array
	A1rim_SIZE,	//sampleindex. if at end of sample array sound is not playing
	41,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"A1rim",	// sample name

	A1shakr,	// pointer to sample array
	A1shakr_SIZE,	// size of the sample array
	A1shakr_SIZE,	//sampleindex. if at end of sample array sound is not playing
	42,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"A1shakr",	// sample name

	B2asnare,	// pointer to sample array
	B2asnare_SIZE,	// size of the sample array
	B2asnare_SIZE,	//sampleindex. if at end of sample array sound is not playing
	43,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2asnare",	// sample name

	B2bd,	// pointer to sample array
	B2bd_SIZE,	// size of the sample array
	B2bd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	44,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2bd",	// sample name

	B2clap,	// pointer to sample array
	B2clap_SIZE,	// size of the sample array
	B2clap_SIZE,	//sampleindex. if at end of sample array sound is not playing
	45,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2clap",	// sample name

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

	B2rim,	// pointer to sample array
	B2rim_SIZE,	// size of the sample array
	B2rim_SIZE,	//sampleindex. if at end of sample array sound is not playing
	49,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2rim",	// sample name

	B2shakr,	// pointer to sample array
	B2shakr_SIZE,	// size of the sample array
	B2shakr_SIZE,	//sampleindex. if at end of sample array sound is not playing
	50,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"B2shakr",	// sample name

	C3asnare,	// pointer to sample array
	C3asnare_SIZE,	// size of the sample array
	C3asnare_SIZE,	//sampleindex. if at end of sample array sound is not playing
	51,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3asnare",	// sample name

	C3bd,	// pointer to sample array
	C3bd_SIZE,	// size of the sample array
	C3bd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	52,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3bd",	// sample name

	C3clap,	// pointer to sample array
	C3clap_SIZE,	// size of the sample array
	C3clap_SIZE,	//sampleindex. if at end of sample array sound is not playing
	53,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3clap",	// sample name

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

	C3rim,	// pointer to sample array
	C3rim_SIZE,	// size of the sample array
	C3rim_SIZE,	//sampleindex. if at end of sample array sound is not playing
	57,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3rim",	// sample name

	C3shakr,	// pointer to sample array
	C3shakr_SIZE,	// size of the sample array
	C3shakr_SIZE,	//sampleindex. if at end of sample array sound is not playing
	58,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"C3shakr",	// sample name

	D4asnare,	// pointer to sample array
	D4asnare_SIZE,	// size of the sample array
	D4asnare_SIZE,	//sampleindex. if at end of sample array sound is not playing
	59,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4asnare",	// sample name

	D4bd,	// pointer to sample array
	D4bd_SIZE,	// size of the sample array
	D4bd_SIZE,	//sampleindex. if at end of sample array sound is not playing
	60,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4bd",	// sample name

	D4clap,	// pointer to sample array
	D4clap_SIZE,	// size of the sample array
	D4clap_SIZE,	//sampleindex. if at end of sample array sound is not playing
	61,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4clap",	// sample name

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

	D4rim,	// pointer to sample array
	D4rim_SIZE,	// size of the sample array
	D4rim_SIZE,	//sampleindex. if at end of sample array sound is not playing
	65,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4rim",	// sample name

	D4shakr,	// pointer to sample array
	D4shakr_SIZE,	// size of the sample array
	D4shakr_SIZE,	//sampleindex. if at end of sample array sound is not playing
	66,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"D4shakr",	// sample name

	Shaker_01,	// pointer to sample array
	Shaker_01_SIZE,	// size of the sample array
	Shaker_01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	67,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Shaker_01",	// sample name

};
