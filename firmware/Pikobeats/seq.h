// sequencer related definitions and structures
#define NTRACKS 8   // we have 8 track sequences
#define MAX_SEQ_STEPS 16 // up to 16 step sequencer
#define DEFAULT_SEQ_STEPS 16 // up to 16 step sequencer

// from pikocore
#define CLOCK_RATE 264000?

#define TEMPO    120 // default tempo
#define PPQN 24  // clocks per quarter note the POs want 2 pulses per 1/4 note. 
#define NOTE_DURATION (PPQN/12) // sixteenth note duration
#define CLOCKPULSE 15 // was 15duration of clock out pulse

int16_t bpm = TEMPO;
int32_t lastMIDIclock; // timestamp of last MIDI clock
int16_t MIDIclocks = PPQN ;//* 2; // midi clock counter
int16_t MIDIsync = 16;  // number of clocks required to sync BPM
int16_t useMIDIclock = 0; // true if we are using MIDI clock
long clocktimer = 0; // clock rate in ms
bool reset = false; // used to reset bpm from CLOCKIN interrupt


// all of the sequencers use the same data structure even though the data may be different in each case
// this simplifies the code somewhat
// clocks are setup for 24ppqn MIDI clock
// note that there are two threads of execution running on the two Pico cores - UI and note handling
// must be careful about editing items that are used by the 2nd Pico core for note timing etc

struct sequencer {
  public: euclid *trigger;  // euclid object to manage hits and patterns from bastl
  uint16_t fills;   // how many hits in 16
  uint16_t repeats;   // set in euclid doStep for now, how often to repeat pattern before new
  uint8_t  count;  // not used, see euclid doStep
  int16_t index;    // index of step we are on
};


sequencer seq[NTRACKS] = {
  new euclid(), 4, 2, 0, DEFAULT_SEQ_STEPS - 1, // step index
  new euclid(), 3, 1, 0, DEFAULT_SEQ_STEPS - 1, // step index
  new euclid(), 2, 3, 0, DEFAULT_SEQ_STEPS - 1, // step index
  new euclid(), 0, 0, 0, DEFAULT_SEQ_STEPS - 1, // step index
  new euclid(), 0, 0, 0, DEFAULT_SEQ_STEPS - 1, // step index
  new euclid(), 0, 0, 0, DEFAULT_SEQ_STEPS - 1, // step index
  new euclid(), 0, 0, 0, DEFAULT_SEQ_STEPS - 1, // step index
  new euclid(), 0, 0, 0, DEFAULT_SEQ_STEPS - 1, // step index
};

// clock all the sequencers
// clockperiod is the period of the 24ppqn clock - used for calculating gate times etc - not used as yet
// it loops thru all tracks looking for trigger events to process
void clocktick (long clockperiod) {

  for (uint8_t track = 0; track < NTRACKS; ++track) { // note that triggers are stored MSB first
    if ( seq[track].trigger->getCurrentStep() ) {
      voice[track].sampleindex = 0; // trigger sample for this track
      voice[track].isPlaying = true;
    } else {
      voice[track].isPlaying = false;
    }
    seq[track].trigger->doStep(); // next step advance
    
  }
}


// must be called regularly for sequencer to run
// hard wired to 16th notes at the moment
void do_clocks(void) {
  //long clockperiod= (long)(((60.0/(float)bpm)/PPQN)*1000);
  long clockperiod = (long)(((60.0 / (float)bpm) / NOTE_DURATION) * 1000);
  
  if ( (millis() - clocktimer) > clockperiod || reset) {
    clocktimer = millis();
    clocktick(clockperiod);
    digitalWrite(CLOCKOUT, 1); // external clock high
    // reset reset for interrupt
    reset = false;
  }
  if ((millis() - clocktimer) > CLOCKPULSE) digitalWrite(CLOCKOUT, 0); // external clock low
}
