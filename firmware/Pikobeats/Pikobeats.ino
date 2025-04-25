/* Copyright 2023 Rich Heslip, 2024 Mark Washeim

  Author: Rich Heslip
  Author: Mark Washeim <blueprint@poetaster.de>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.

  See http://creativecommons.org/licenses/MIT/ for more information.

  -----------------------------------------------------------------------------


  // sample player inspired by Jan Ostman's ESP8266 drum machine http://janostman.wordpress.com

  samples for beatbox from:
  giddster ( https://freesound.org/people/giddster/ )
  AlienXXX ( https://freesound.org/people/AlienXXX/
  
  The euclid code originates at:
  https://github.com/bastl-instruments/one-chip-modules/blob/master/EUCLID/EUCLID.ino

*/

#include <Arduino.h>
#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/sio.h"
//#include "MIDI.h"
#include <PWMAudio.h>
#include "io.h"
#include "euclid.h"
#include "filter.h"

// additions
#include <Wire.h>
#include <RotaryEncoder.h>
//#include <Bounce2.h>



// display setup works with adafruit SSD1306 or SH1106G
const int dw = 128;
const int dh = 64;
const int oled_sda_pin = 20;
const int oled_scl_pin = 21;
const int oled_i2c_addr = 0x3C;
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

//#include <Adafruit_SSD1306.h>
//Adafruit_SSD1306 display(dw, dh, &Wire, OLED_RESET);
//#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


#include <Adafruit_SH110X.h>
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire);
#define WHITE SH110X_WHITE

#include "font.h"
#include "helvnCB6pt7b.h"
#define myfont Org_01 //helvnCB6pt7b // Org_01 looks better but is small.
#define bigfont helvnCB6pt7b

// from pikocore for bpm calcs on clk input
// this is unused, deprecate?
#include "runningavg.h"
RunningAverage ra;
volatile int clk_display;
uint32_t clk_sync_last;

// clock timer 
#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     4
// Can be included as many times as necessary, without `Multiple Definitions` Linker Error
#include "RPi_Pico_TimerInterrupt.h"
unsigned int SWPin = CLOCKIN;
#define TIMER0_INTERVAL_MS       1
#define DEBOUNCING_INTERVAL_MS   2// 80
#define LOCAL_DEBUG              1

// Init RPI_PICO_Timer, can use any from 0-15 pseudo-hardware timers
RPI_PICO_Timer ITimer0(0);

volatile unsigned long rotationTime = 0;
float RPM       = 0.00;
float avgRPM    = 0.00;
volatile int debounceCounter;

int current_track = 0; // track we are working on
// input clk tracking
volatile int clk_state_last; // track the CLOCKIN pin state.

int clk_state = 0;
int clk_hits = 0;
uint32_t clk_sync_ms = 0;
bool sync = false; // used to detect if we have input sync


bool TimerHandler0(struct repeating_timer *t)
{  
  (void) t;  
  //if( digitalRead(SWPin) && clk_state_last != digitalRead(SWPin) && debounceCounter >= DEBOUNCING_INTERVAL_MS)
  if( digitalRead(SWPin) && clk_state_last != digitalRead(SWPin))
  {
    //min time between pulses has passed
    // calculate bpm
    RPM = (float) ( 60000.0f / ( rotationTime * TIMER0_INTERVAL_MS ) / 2.0f );
    //use running avg NOT volatile on timer
    //ra.Update(RPM);
    clk_display = RPM;
    // these are for the sequencer
    sync = true;

#if (TIMER_INTERRUPT_DEBUG > 0)
      Serial.print("rt = "); Serial.print(RPM);
      //Serial.print("RPM = "); Serial.print(ra.Value());
      Serial.print(", rotationTime ms = "); Serial.println(rotationTime * TIMER0_INTERVAL_MS);
#endif
    rotationTime = 0;
    debounceCounter = 0;
  }
  else
  {
    debounceCounter++;
  }
  if (rotationTime >= 1000)
  {
    // If idle, set RPM to 0, don't increase rotationTime
    sync = false ;// flag for seq.h

    RPM = 0;
#if (TIMER_INTERRUPT_DEBUG > 0)   
    Serial.print("RPM = "); Serial.print(RPM); Serial.print(", rotationTime = "); Serial.println(rotationTime);
#endif  
    rotationTime = 0;
  }
  else
  {
    rotationTime++;
  }
  clk_state_last = digitalRead(SWPin);
  return true;
}

// begin hardware definitions
const int key_pins[] = { 0, 2, 4, 6, 8, 10, 12, 14 };
const int led_pins[] = { 1, 3, 5, 7, 9, 11, 13, 15 };

// on the long ec11 these are swapped A 19, B 18
const int encoderA_pin = 19;
const int encoderB_pin = 18;
const int encoderSW_pin = 28;


// encoder
RotaryEncoder encoder(encoderB_pin, encoderA_pin, RotaryEncoder::LatchMode::FOUR3);

void checkEncoderPosition() {
  encoder.tick();   // call tick() to check the state.
}

// variables for UI state management
int encoder_pos_last = 0;
int encoder_delta = 0;
uint32_t encoder_push_millis;
uint32_t step_push_millis;

// currently not used
int step_push = -1;
bool step_edited = false;
char seq_info[11];  // 10 chars + nul FIXME

bool encoder_held = false;

// pots
#define NPOTS 2 // number of pots
uint16_t potvalue[NPOTS]; // pot readings
uint16_t lastpotvalue[NPOTS]; // old pot readings
bool potlock[NPOTS]; // when pots are locked it means they must change by MIN_POT_CHANGE to register
uint32_t pot_timer; // reading pots too often causes noise
#define POT_SAMPLE_TIME 30 // delay time between pot reads
#define MIN_POT_CHANGE 25 // locked pot reading must change by this in order to register
#define MIN_COUNTS 8  // unlocked pot must change by this in order to register
#define POT_AVERAGING 20 // analog sample averaging count 
#define POT_MIN 4   // A/D may not read min value of 0 so use a bit larger value for map() function
#define POT_MAX 1019 // A/D may not read max value of 1023 so use a bit smaller value for map() function


// buttons

#define NUM_BUTTONS 9 // 8 buttons plus USR button on VCC-GND board
#define SHIFT 8 // index of "shift" USR button 
uint8_t debouncecnt[NUM_BUTTONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; // debounce counters
bool button[NUM_BUTTONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; // key active flags
int led[8] = {LED0, LED1, LED2, LED3, LED4, LED5, LED6, LED7};
int buttons[NUM_BUTTONS] = {BUTTON0, BUTTON1, BUTTON2, BUTTON3, BUTTON4, BUTTON5, BUTTON6, BUTTON7, SHIFT};

/* no MIDI for now
  // MIDI stuff
  uint8_t MIDI_Channel=10;  // default MIDI channel for percussion
  struct SerialMIDISettings : public midi::DefaultSettings
  {
  static const long BaudRate = 31250;
  };
  // must use HardwareSerial for extra UARTs
  HardwareSerial MIDISerial(2);

  // instantiate the serial MIDI library
  MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, MIDISerial, MIDI, SerialMIDISettings);
*/


enum {
  MODE_PLAY = 0,
  MODE_CONFIG,
  MODE_COUNT   // how many modes we got
};

int display_mode = MODE_PLAY;
uint8_t display_repeats = 0;
uint8_t display_vol = 100;
uint8_t display_pitch = 50;
String display_pat;

// --- display details from picostep
//
typedef struct {
  int x;
  int y;
  const char* str;
} pos_t;

const char* note_strs[] = { "C ", "C#", "D ", "D# ", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B ", "C " };

int notenum_to_oct(int notenum) {
  return (notenum / 12) - 2;
}
const char* notenum_to_notestr(int notenum) {
  return note_strs[notenum % 12];
}
// END additions

// from pikocore filter
uint8_t filter_fc = LPF_MAX + 10;
uint8_t hpf_fc = 0;
uint8_t filter_q = 0;


//#define MONITOR_CPU  // define to monitor Core 2 CPU usage on pin CPU_USE

#define SAMPLERATE 22050
//#define SAMPLERATE 44100 // VCC-GND 16mb flash boards won't overclock fast enough for 44khz ?

PWMAudio DAC(PWMOUT);  // 16 bit PWM audio



// flag all pot values as locked ie they have to change more than MIN_POT_CHANGE to register
void lockpots(void) {
  for (int i = 0; i < NPOTS; ++i) potlock[i] = 1;
}

// sample analog pot input and do filtering.
// if pots are locked, change value only if movement is greater than MIN_POT_CHANGE
uint16_t readpot(uint8_t potnum) {
  int val = 0;
  int input;
  switch (potnum) { // map potnum to RP2040 pin
    case 0:
      input = AIN0;
      break;
    case 1:
      input = AIN1;
      break;
    case 2:
    default:   // shouldn't happen
      input = AIN0;
      break;
  }
  // note that Pikocore pots are wired "backwards" - max voltage is full ccw
  for (int j = 0; j < POT_AVERAGING; ++j) val += (1024 - analogRead(input)); // read the A/D a few times and average for a more stable value
  val = val / POT_AVERAGING;
  if (potlock[potnum]) {
    int delta = lastpotvalue[potnum] - val; // this needs to be done outside of the abs() function - see arduino abs() docs
    if (abs(delta) > MIN_POT_CHANGE) {
      potlock[potnum] = 0; // flag pot no longer locked
      potvalue[potnum] = lastpotvalue[potnum] = val; // save the new reading
    }
    else val = lastpotvalue[potnum];
  }
  else {
    if (abs(lastpotvalue[potnum] - val) > MIN_COUNTS) lastpotvalue[potnum] = val; // even if pot is unlocked, make sure pot has moved at least MIN_COUNT counts so values don't jump around
    else val = lastpotvalue[potnum];
    potvalue[potnum] = val; // pot is unlocked so save the reading
  }
  return val;
}




// we have 8 voices that can play any sample when triggered
// this structure holds the settings for each voice
// 80s only to 20, jungle to 29

//we use a header per sample set
#include "80s.h"
//#include "beatbox.h"
//#include "bbox.h"
//#include "angularj.h"

// we can have an arbitrary number of samples but you will run out of memory at some point
// sound sample files are 22khz 16 bit signed PCM format - see the sample include files for examples
// you can change the sample rate to whatever you want but most testing was done at 22khz. 44khz probably works but not much testing was done
// use the wave2header22khz.exe utility to automagically batch convert all the .wav files in a directory into the required header files
// put your 22khz or 44khz PCM wav files in a sample subdirectory with a copy of the utility, run the utility and it will generate all the required header files
// wave2header creates a header file containing the signed PCM values for each sample - note that it may change the name of the file if required to make it "c friendly"
// wave2header also creates sampledefs.h which is an array of structures containing information about each sample file
// the samples are arranged in alphabetical order to facilitate grouping samples by name - you can manually edit this file to change the order of the samples as needed
// sampledefs.h contains other information not used by this program e.g. the name of the sample file - I wrote it for another project
// wave2header also creates "samples.h" which #includes all the generated header files
//#include "Jungle/samples.h"
//#include "808samples/samples.h" // 808 sounds
//#include "Angular_Jungle_Set/samples.h"   // Jungle soundfont set - great!
//#include "Angular_Techno_Set/samples.h"   // Techno
//#include "Acoustic3/samples.h"   // acoustic drums
//#include "Pico_kit/samples.h"   // assorted samples
//#include "testkit/samples.h"   // small kit for testing
//#include "Trashrez/samples.h"
//#include "world/samples.h"
//#include "mt40sr88sy1/samples.h"
//#include "kurzweill/samples.h"
//#include "beatbox/samples.h"
//#include "bbox/samples.h"


#define NUM_SAMPLES (sizeof(sample)/sizeof(sample_t))

// sample and debounce the keys


// scan buttons
bool scanbuttons(void)
{
  bool pressed;
  for (int i = 0; i < NUM_BUTTONS; ++i) {
    switch (i) { // sample gate inputs
      case 0:
        pressed = !digitalRead(BUTTON0); // active low key inputs
        break;
      case 1:
        pressed = !digitalRead(BUTTON1);
        break;
      case 2:
        pressed = !digitalRead(BUTTON2);
        break;
      case 3:
        pressed = !digitalRead(BUTTON3);
        break;
      case 4:
        pressed = !digitalRead(BUTTON4);
        break;
      case 5:
        pressed = !digitalRead(BUTTON5);
        break;
      case 6:
        pressed = !digitalRead(BUTTON6);
        break;
      case 7:
        pressed = !digitalRead(BUTTON7);
        break;
      case 8:
        pressed = !digitalRead(SHIFTBUTTON);
        break;
    }

    if (pressed) {
      if (debouncecnt[i] <= 3) ++debouncecnt[i];
      if (debouncecnt[i] == 2) { // trigger on second sample of key active
        button[i] = 1;
      }
    }
    else {
      debouncecnt[i] = 0;
      button[i] = 0;
    }
  }
  if (pressed) return true;
  else return false;
}


/* no MIDI for now
  // serial MIDI handler

  void HandleNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
  if (channel==MIDI_Channel) {
      for (int i=0; i< NUM_SAMPLES;++i) {  //
        if (sample[i].MIDINOTE == note) {
          sample[i].play_volume=velocity;  // use MIDI volume
          sample[i].sampleindex=0;  // if note matches sample MIDI note number, start it playing
        }
      }
  }
  }
*/

// include here to avoid forward references - I'm lazy :)

#include "seq.h"

#define DISPLAY_TIME 2000 // time in ms to display numbers on LEDS
int32_t display_timer;

// show a number in binary on the LEDs
void display_value(int16_t value) {
  for (int i = 7; i >= 0; i--) { // NOPE + 1 can loop this way because port assignments are sequential
    digitalWrite(led[i], value & 1);
    value = value >> 1;
  }
  display_timer = millis();
}

// rotate trigger pattern
uint16_t rightRotate(int shift, uint16_t value, uint8_t pattern_length) {
  uint16_t mask = ((1 << pattern_length) - 1);
  value &= mask;
  return ((value >> shift) | (value << (pattern_length - shift))) & mask;
}

// main core setup
void setup() {
  // set clock speed as in picokore
  //set_sys_clock_khz(264000, true); don't do this :)

  Serial.begin(115200);
  Serial.print(F("\nStarting RPM_Measure on ")); Serial.println(BOARD_NAME);
  Serial.println(RPI_PICO_TIMER_INTERRUPT_VERSION);
  Serial.print(F("CPU Frequency = ")); Serial.print(F_CPU / 1000000); Serial.println(F(" MHz"));
    // Interval in microsecs
  if(ITimer0.attachInterruptInterval(TIMER0_INTERVAL_MS * 1000, TimerHandler0))
  {
    Serial.print(F("Starting  ITimer0 OK, millis() = ")); Serial.println(millis());
  }  else {
    Serial.println(F("Can't set ITimer0. Select another freq. or timer"));
   }
  Serial.flush();  
  
  // Additions
  // ENCODER
  pinMode(encoderA_pin, INPUT_PULLUP);
  pinMode(encoderB_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoderA_pin), checkEncoderPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderB_pin), checkEncoderPosition, CHANGE);

  // DISPLAY need to tell Wire which pins for i2c
  Wire.setSDA(oled_sda_pin);
  Wire.setSCL(oled_scl_pin);
  Wire.begin();

  // SSD1306 -- 
  //if (!display.begin(SSD1306_SWITCHCAPVCC, oled_i2c_addr)) {
  if (!display.begin( oled_i2c_addr)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;) ;  // Don't proceed, loop forever
  }

  displaySplash();
  //displayConfig();

  // end Additions

#ifdef MONITOR_CPU
  pinMode(CPU_USE, OUTPUT); // for monitoring CPU usage
#endif

  pinMode(BUTTON0, INPUT_PULLUP);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);
  pinMode(BUTTON5, INPUT_PULLUP);
  pinMode(BUTTON6, INPUT_PULLUP);
  pinMode(BUTTON7, INPUT_PULLUP);
  pinMode(SHIFTBUTTON, INPUT_PULLUP);

  pinMode(AIN0, INPUT);
  pinMode(AIN1, INPUT);

  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
  pinMode(LED7, OUTPUT);

  pinMode(CLOCKOUT, OUTPUT);
  pinMode(CLOCKIN, INPUT_PULLUP);
  
  pinMode(23, OUTPUT); // thi is to switch to PWM for power to avoid ripple noise
  digitalWrite(23, HIGH);
  
  // set up Pico PWM audio output
  DAC.setBuffers(4, 128); // DMA buffers
  DAC.begin(SAMPLERATE);

  /* no MIDI for now
    //  Set up serial MIDI port
    MIDISerial.begin(31250, SERIAL_8N1, MIDIRX,MIDITX ); // midi port

    // set up serial MIDI library callbacks
    MIDI.setHandleNoteOn(HandleNoteOn);  //

    // Initiate serial MIDI communications, listen to all channels
    MIDI.begin(MIDI_CHANNEL_OMNI);
  */
  // set up runningavg
  ra.Init(5);
  
  seq[0].trigger->generateRandomSequence(8, 16);
  seq[2].trigger->generateRandomSequence(3, 16);
  seq[5].trigger->generateRandomSequence(5, 16);
  seq[7].trigger->generateRandomSequence(6, 16);
  display_value(NUM_SAMPLES); // show number of samples on the display

}



// main core handles UI
void loop() {
  bool anybuttonpressed;
  // timer
  uint32_t now = millis();


  // UI handlers
  // first encoder
  encoder.tick();
  int encoder_pos = encoder.getPosition();
  if ( (encoder_pos != encoder_pos_last )) {
    encoder_delta = encoder_pos - encoder_pos_last;
  }


  // set play mode 0 play 1 edit patterns, 3 FX?
  if (encoder_push_millis > 0 ) {
    if ((now - encoder_push_millis) > 25 && ! encoder_delta ) {
      if ( !encoder_held ) {
        encoder_held = true;
        display_mode = display_mode+1;
        if ( display_mode > 2) { // switched back to play mode
          display_mode = 0;
          //configure_sequencer();
        }
      }
    }

    if (step_push_millis > 0) { // we're pushing a step key too
      if (encoder_push_millis < step_push_millis) {  // and encoder was pushed first
        //strcpy(seq_info, "saveseq");
      }
    }
  }

  anybuttonpressed = false;

  for (int i = 0; i <= 8; ++i) { // scan all the buttons
    if (button[i]) {

      anybuttonpressed = true;

      // a track button is pressed
      current_track = i; // keypress selects track we are working on
      
      //  if ((!potlock[1]) || (!potlock[2])) seq[i].trigger=euclid(16,map(potvalue[1],POT_MIN,POT_MAX,0,MAX_SEQ_STEPS),map(potvalue[2],POT_MIN,POT_MAX,0,MAX_SEQ_STEPS-1));

      // look up drum trigger pattern encoder play modes
      if ( (encoder_pos != encoder_pos_last ) && ! button[8] && display_mode == 0) {
        rp2040.idleOtherCore();
        int result = voice[i].sample + encoder_delta;
        if (result >= 0 && result <= NUM_SAMPLES - 1) {
          voice[i].sample = result;
        }
        rp2040.resumeOtherCore();

      }

      if ( (encoder_pos != encoder_pos_last ) && display_mode == 1 ) {
        //uint8_t re = seq[i].trigger->getRepeats() + encoder_delta;
        seq[i].trigger->setRepeats(encoder_delta);
        display_repeats = seq[i].trigger->getRepeats();

      }

      // change pitch on pot 0
      if (!potlock[0] && ( display_mode == 0 || display_mode ==2) ){ // change sample if pot has moved enough
        uint16_t pitch = (uint16_t)(map(potvalue[0], POT_MIN, POT_MAX, 2048, 8192));
        // divisible by 2 and it won't click
        if (pitch%2 == 0) { 
          voice[current_track].sampleincrement = pitch;  // change sample pitch if pot has moved enough
          display_pitch = map(pitch, 2048, 8192, 0,100);
        }
      }

      // change volume on pot 1
      if (!potlock[1] && display_mode == 0) {
        int16_t level = (int16_t)(map(potvalue[1], POT_MIN, POT_MAX, 0, 1000));
        voice[current_track].level = level;
        display_vol = level/10;
        // change sample volume level if pot has moved enough
      }
      if (!potlock[0] && display_mode == 1 ) {
        //filter_fc = potvalue[0] * (LPF_MAX + 10) / 4096;
        seq[i].fills = map(potvalue[0], POT_MIN, POT_MAX, 0, 16);
        seq[i].trigger->generateRandomSequence(seq[i].fills, 15);
        display_pat = (String) seq[i].trigger->textSequence;
      }

      // set track euclidean triggers if either pot has moved enough
      if (!potlock[1] && ! button[8] && display_mode == 1) {
        seq[i].fills = map(potvalue[1], POT_MIN, POT_MAX, 0, 16);
        seq[i].trigger->generateSequence(seq[i].fills, 15);
        seq[i].trigger->resetSequence(); // set to 0
        display_pat = (String) seq[i].trigger->textSequence;
        
      }
      //trig/retrig play
      if ( display_mode == 2 && i < 8 && voice[current_track].isPlaying == false) {
         voice[current_track].sampleindex = 0; // trigger sample for this track
         voice[current_track].isPlaying = true;
      }
      /*
        if (!potlock[2]) {
        seq[i].trigger->setRepeats(map(potvalue[2], POT_MIN, POT_MAX, 0, 32));
        //seq[i].trigger= rightRotate(map(potvalue[2],POT_MIN,POT_MAX,0,MAX_SEQ_STEPS-1),seq[i].trigger,16); // rotate trigger pattern
        }
      */

    }
  }

  // now, after buttons check if only encoder moved and no buttons
  if (! anybuttonpressed && encoder_delta ) {
    bpm = bpm + encoder_delta;
    displayUpdate();
    display_value(bpm - 50);
  }

  /// only set new pos last after buttons have had a chance to use the delta
  encoder_pos_last = encoder_pos;
  encoder_delta = 0;  // we've used it

  // start tracking time encoder button held
  if (button[8]) {
    encoder_push_millis = now;
  } else {
    encoder_push_millis = 0;
    encoder_held = false;
  }

  // lock pot settings when no keys are pressed so it requires more movement to change value
  // this is so when we change tracks we don't immediately change the settings on the new track
  if (!anybuttonpressed) lockpots();

  // MIDI.read();  // do serial MIDI
  
  scanbuttons();

  // reading A/D seems to cause noise in the audio so don't do it too often
  if ((now - pot_timer) > POT_SAMPLE_TIME) {
    readpot(0);
    readpot(1);
    pot_timer = now;
  }

  // if display is not busy show track triggers on leds
  if ((now - display_timer) > DISPLAY_TIME) {
    displayUpdate();
    for (int i = 0; i <= 7; ++i) { // LED port numbers are sequential on the Pikocore
      if ( seq[i].trigger->getCurrentStep() ) digitalWrite(led[i], 1);
      else digitalWrite(led[i], 0);
    }
  }
}

// second core setup
// second core dedicated to sample processing
void setup1() {
  delay (2000); // wait for main core to start up perhipherals
}

// second core calculates samples and sends to DAC
void loop1() {

  // check if we have a new bpm value from interrupt
  // since debouncing is flaky, force more than 1 bpm diff
    //if (ra.Value() != bpm && ra.Value() > 49) {
  if (RPM > bpm + 1 || RPM < bpm -1 && RPM > 49) {
        //reset = true; //reset seq
        bpm = RPM;
  }
  do_clocks();  // process sequencer clocks
  

  int32_t newsample, samplesum = 0, filtersum;
  uint32_t index;
  int16_t samp0, samp1, delta, tracksample;

  /* oct 22 2023 resampling code
     to change pitch we step through the sample by .5 rate for half pitch up to 2 for double pitch
     sample.sampleindex is a fixed point 20:12 fractional number
     we step through the sample array by sampleincrement - sampleincrement is treated as a 1 bit integer and a 12 bit fraction
     for sample lookup sample.sampleindex is converted to a 20 bit integer which limits the max sample size to 2**20 or about 1 million samples, about 45 seconds
  */
  for (int track = 0; track < NTRACKS; ++track) { // look for samples that are playing, scale their volume, and add them up
    tracksample = voice[track].sample; // precompute for a little more speed below
    index = voice[track].sampleindex >> 12; // get the integer part of the sample increment
    if (index >= sample[tracksample].samplesize) voice[track].isPlaying = false; // have we played the whole sample?
    if (voice[track].isPlaying) { // if sample is still playing, do interpolation
      samp0 = sample[tracksample].samplearray[index]; // get the first sample to interpolate
      samp1 = sample[tracksample].samplearray[index + 1]; // get the second sample
      delta = samp1 - samp0;
      newsample = (int32_t)samp0 + ((int32_t)delta * ((int32_t)voice[track].sampleindex & 0x0fff)) / 4096; // interpolate between the two samples
      //samplesum+=((int32_t)samp0+(int32_t)delta*(sample[i].sampleindex & 0x0fff)/4096)*sample[i].play_volume;
      samplesum += (newsample * (127 * voice[track].level)) / 1000;
      voice[track].sampleindex += voice[track].sampleincrement; // add step increment
    }
  }

  samplesum = samplesum >> 7; // adjust for play_volume multiply above
  if  (samplesum > 32767) samplesum = 32767; // clip if sample sum is too large
  if  (samplesum < -32767) samplesum = -32767;

  /*
      // filter
    if (filter_fc <=  LPF_MAX) {
        filtersum = (uint8_t)filter_lpf( (int64_t)samplesum, filter_fc ,filter_q);
     }
  */

#ifdef MONITOR_CPU
  digitalWrite(CPU_USE, 0); // low - CPU not busy
#endif
  // write samples to DMA buffer - this is a blocking call so it stalls when buffer is full
  DAC.write(int16_t(samplesum)); // left

#ifdef MONITOR_CPU
  digitalWrite(CPU_USE, 1); // hi = CPU busy
#endif


}


// ADD
//

//// {x,y} locations of play screen items
const int step_text_pos[] = { 0, 15, 16, 15, 32, 15, 48, 15, 64, 15, 80, 15, 96, 15, 112, 15 };
const pos_t bpm_text_pos    = {.x = 0,  .y = 15, .str = "bpm:%3d" };
const pos_t trans_text_pos  = {.x = 65, .y = 15, .str = "trs:%+2d" };
const pos_t seqno_text_pos  = {.x = 65, .y = 30, .str = "seq:%d" };
const pos_t seq_info_pos    = {.x = 0, .y = 30, .str = "" };
const pos_t play_text_pos   = {.x = 0, .y = 45, .str = "" };
const pos_t pat_text_pos    = {.x = 0, .y = 60,  .str = "" };

const pos_t gate_bar_offset = { .x = 0, .y = -15, .str = "" };
const pos_t edit_text_offset = { .x = 3, .y = 22,  .str = "" };
const int gate_bar_width = 14;
const int gate_bar_height = 4;

void displayUpdate() {
  display.clearDisplay();
  //display.setFont(&myfont); //don't need to call this every time!
  display.cp437(true); 
  //display.setTextColor(WHITE, 0);
  /*
    for (int i = 0; i < 8; i++) {
    Step s = seqr.steps[i];
    const char* nstr = notenum_to_notestr(s.note);
    int o = notenum_to_oct(s.note);
    int x = step_text_pos[i * 2], y = step_text_pos[i * 2 + 1];
    display.setCursor(x, y);
    display.print(nstr);
    display.setCursor(x + oct_text_offset.x, y + oct_text_offset.y);
    display.printf("%1d", o);
    display.setCursor(x + edit_text_offset.x, y + edit_text_offset.y);
    display.print((i == selected_step) ? '^' : (s.on) ? ' '
                                                      : '*');
    int gate_w = 1 + (s.gate * gate_bar_width / 16);
    display.fillRect(x + gate_bar_offset.x, y + gate_bar_offset.y, gate_w, gate_bar_height, WHITE);
    }
  */

  // display.setFont(&myfont2);
  /*
    Serial.print("eb1 incremented by: ");
    Serial.println(eb.increment());
    Serial.print("eb1 position is: ");
    Serial.println(eb.position());
    Serial.print("eb1 clickCount: ");
    Serial.println(eb.clickCount());
  */

  /*
       const pos_t bpm_text_pos    = {.x=0,  .y=57, .str="bpm:%3d" };
    const pos_t trans_text_pos  = {.x=55, .y=57, .str="trs:%+2d" };
    const pos_t seqno_text_pos  = {.x=0,  .y=45, .str="seq:%d" };
    const pos_t seq_info_pos    = {.x=60, .y=45, .str="" };
    const pos_t play_text_pos   = {.x=110,.y=57, .str="" };
  */
  // bpm
  display.setCursor(bpm_text_pos.x, bpm_text_pos.y);
  display.print("BPM: ");
  display.print(bpm);

  // transpose
  display.setCursor(trans_text_pos.x, trans_text_pos.y);
  display.print("VOL: ");
  display.print(display_vol);
  
  // seq info / meta
  display.setCursor(seq_info_pos.x, seq_info_pos.y);
  display.print("REPS: ");
  display.print(display_repeats);
  
  // seqno
  display.setCursor(seqno_text_pos.x, seqno_text_pos.y);
  display.print("PITCH: ");
  display.print(display_pitch);  // user sees 1-8
  
  // seq info / meta
  display.setCursor(play_text_pos.x, play_text_pos.y);
  display.print("MODE: ");
  display.print(display_mode);
  
  // although cool, can't read 01 in org font
  //display.setFont(&bigfont); //don't need to call this every time!
  display.setCursor(pat_text_pos.x, pat_text_pos.y);
  display.print("PAT: ");
  display.print((String)display_pat);
  
  // play/pause
  //display.setCursor(play_text_pos.x, play_text_pos.y);
  //display.print(seqr.playing ? " >" : "[]");

  display.display();
}

void displaySplash() {
  display.clearDisplay();
  display.setFont(&myfont);
  display.setTextColor(WHITE, 0);
  display.drawRect(0, 0, dw - 1, dh - 1, WHITE);
  display.setCursor(25, 32);
  display.print("PikoBeatBox");
  display.display();
}
