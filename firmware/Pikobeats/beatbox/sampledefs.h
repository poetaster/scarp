// sample structure built by wav2header based on wav2sketch by Paul Stoffregen

struct sample_t {
  const int16_t * samplearray; // pointer to sample array
  uint32_t samplesize; // size of the sample array
  uint32_t sampleindex; // current sample array index when playing. index at last sample= not playing
  uint8_t MIDINOTE;  // MIDI note on that plays this sample
  uint8_t play_volume; // play volume 0-127
  char sname[20];        // sample name
} sample[] = {

	Bass_001,	// pointer to sample array
	Bass_001_SIZE,	// size of the sample array
	Bass_001_SIZE,	//sampleindex. if at end of sample array sound is not playing
	35,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Bass_001",	// sample name

	Bblip1,	// pointer to sample array
	Bblip1_SIZE,	// size of the sample array
	Bblip1_SIZE,	//sampleindex. if at end of sample array sound is not playing
	36,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Bblip1",	// sample name

	Bbox_hat_01,	// pointer to sample array
	Bbox_hat_01_SIZE,	// size of the sample array
	Bbox_hat_01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	37,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Bbox_hat_01",	// sample name

	Bbox_hat_09,	// pointer to sample array
	Bbox_hat_09_SIZE,	// size of the sample array
	Bbox_hat_09_SIZE,	//sampleindex. if at end of sample array sound is not playing
	38,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Bbox_hat_09",	// sample name

	Bbox_kick_11,	// pointer to sample array
	Bbox_kick_11_SIZE,	// size of the sample array
	Bbox_kick_11_SIZE,	//sampleindex. if at end of sample array sound is not playing
	39,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Bbox_kick_11",	// sample name

	Bbox_perc_03,	// pointer to sample array
	Bbox_perc_03_SIZE,	// size of the sample array
	Bbox_perc_03_SIZE,	//sampleindex. if at end of sample array sound is not playing
	40,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Bbox_perc_03",	// sample name

	Bbox_perc_04,	// pointer to sample array
	Bbox_perc_04_SIZE,	// size of the sample array
	Bbox_perc_04_SIZE,	//sampleindex. if at end of sample array sound is not playing
	41,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Bbox_perc_04",	// sample name

	Bbox_perc_06,	// pointer to sample array
	Bbox_perc_06_SIZE,	// size of the sample array
	Bbox_perc_06_SIZE,	//sampleindex. if at end of sample array sound is not playing
	42,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Bbox_perc_06",	// sample name

	Clap_004,	// pointer to sample array
	Clap_004_SIZE,	// size of the sample array
	Clap_004_SIZE,	//sampleindex. if at end of sample array sound is not playing
	43,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Clap_004",	// sample name

	Clap_07,	// pointer to sample array
	Clap_07_SIZE,	// size of the sample array
	Clap_07_SIZE,	//sampleindex. if at end of sample array sound is not playing
	44,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Clap_07",	// sample name

	Closedhat_003,	// pointer to sample array
	Closedhat_003_SIZE,	// size of the sample array
	Closedhat_003_SIZE,	//sampleindex. if at end of sample array sound is not playing
	45,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Closedhat_003",	// sample name

	Giddrew,	// pointer to sample array
	Giddrew_SIZE,	// size of the sample array
	Giddrew_SIZE,	//sampleindex. if at end of sample array sound is not playing
	46,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Giddrew",	// sample name

	Gidstck,	// pointer to sample array
	Gidstck_SIZE,	// size of the sample array
	Gidstck_SIZE,	//sampleindex. if at end of sample array sound is not playing
	47,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Gidstck",	// sample name

	Gidtom5,	// pointer to sample array
	Gidtom5_SIZE,	// size of the sample array
	Gidtom5_SIZE,	//sampleindex. if at end of sample array sound is not playing
	48,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Gidtom5",	// sample name

	Gidtt1,	// pointer to sample array
	Gidtt1_SIZE,	// size of the sample array
	Gidtt1_SIZE,	//sampleindex. if at end of sample array sound is not playing
	49,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Gidtt1",	// sample name

	Gidtt2,	// pointer to sample array
	Gidtt2_SIZE,	// size of the sample array
	Gidtt2_SIZE,	//sampleindex. if at end of sample array sound is not playing
	50,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Gidtt2",	// sample name

	Gidtzz2,	// pointer to sample array
	Gidtzz2_SIZE,	// size of the sample array
	Gidtzz2_SIZE,	//sampleindex. if at end of sample array sound is not playing
	51,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Gidtzz2",	// sample name

	Gidtzz,	// pointer to sample array
	Gidtzz_SIZE,	// size of the sample array
	Gidtzz_SIZE,	//sampleindex. if at end of sample array sound is not playing
	52,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Gidtzz",	// sample name

	Gidwoop,	// pointer to sample array
	Gidwoop_SIZE,	// size of the sample array
	Gidwoop_SIZE,	//sampleindex. if at end of sample array sound is not playing
	53,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Gidwoop",	// sample name

	Gidzz2,	// pointer to sample array
	Gidzz2_SIZE,	// size of the sample array
	Gidzz2_SIZE,	//sampleindex. if at end of sample array sound is not playing
	54,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Gidzz2",	// sample name

	Hhat1,	// pointer to sample array
	Hhat1_SIZE,	// size of the sample array
	Hhat1_SIZE,	//sampleindex. if at end of sample array sound is not playing
	55,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Hhat1",	// sample name

	Kick_007,	// pointer to sample array
	Kick_007_SIZE,	// size of the sample array
	Kick_007_SIZE,	//sampleindex. if at end of sample array sound is not playing
	56,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Kick_007",	// sample name

	Rim1,	// pointer to sample array
	Rim1_SIZE,	// size of the sample array
	Rim1_SIZE,	//sampleindex. if at end of sample array sound is not playing
	57,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Rim1",	// sample name

	Snr_3,	// pointer to sample array
	Snr_3_SIZE,	// size of the sample array
	Snr_3_SIZE,	//sampleindex. if at end of sample array sound is not playing
	58,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Snr_3",	// sample name

	Uup,	// pointer to sample array
	Uup_SIZE,	// size of the sample array
	Uup_SIZE,	//sampleindex. if at end of sample array sound is not playing
	59,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Uup",	// sample name

	Whis1,	// pointer to sample array
	Whis1_SIZE,	// size of the sample array
	Whis1_SIZE,	//sampleindex. if at end of sample array sound is not playing
	60,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Whis1",	// sample name

	Whis2,	// pointer to sample array
	Whis2_SIZE,	// size of the sample array
	Whis2_SIZE,	//sampleindex. if at end of sample array sound is not playing
	61,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Whis2",	// sample name

	Zbcrk,	// pointer to sample array
	Zbcrk_SIZE,	// size of the sample array
	Zbcrk_SIZE,	//sampleindex. if at end of sample array sound is not playing
	62,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Zbcrk",	// sample name

};
