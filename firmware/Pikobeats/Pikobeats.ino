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
  // completely rewritten for the Motivation Radio Eurorack module
  // plays samples in response to gate/trigger inputs and MIDI note on messages
  // will play any 22khz sample file converted to a header file in the appropriate format
  // Feb 3/19 - initial version
  // Feb 11/19 - sped up encoder/trigger ISR so it will catch 1ms pulses from Grids
  // Jan 2023 - ported code to Pi Pico so I can use it on a 16mb flash version
  // oct 2023 - ported to Pikocore Hardware and converted to a simple beatbox
  // Feb 2024 - added step clock out
  // Feb 2024 - blueprint replaced pattern logic.
  // Feb 2024 - blueprint add  beatbox kit

  samples from:
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

// additions
#include <Wire.h>
#include <RotaryEncoder.h>
//#include <Bounce2.h>
#include <Adafruit_SSD1306.h>

#include "font.h"
#define myfont Org_01


// begin hardware definitions
const int dw = 128;
const int dh = 64;

const int key_pins[] = { 0, 2, 4, 6, 8, 10, 12, 14 };
const int led_pins[] = { 1, 3, 5, 7, 9, 11, 13, 15 };

const int encoderA_pin = 19;
const int encoderB_pin = 18;
const int encoderSW_pin = 28;

const int oled_sda_pin = 20;
const int oled_scl_pin = 21;

const int oled_i2c_addr = 0x3C;

//Adafruit_SSD1306 display(dw, dh, &Wire, -1);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//EncoderButton eb1(19,20,28);
//Bounce2::Button encoder_switch;
RotaryEncoder encoder(encoderB_pin, encoderA_pin, RotaryEncoder::LatchMode::FOUR3);

void checkEncoderPosition() {
  encoder.tick();   // call tick() to check the state.
}

int encoder_pos_last = 0;

// variables for UI state management
int encoder_delta = 0;
uint32_t encoder_push_millis;
uint32_t step_push_millis;
//bool steps_pushed[numsteps]; // which keys are pressed
int step_push = -1;
bool step_edited = false;
char seq_info[11];  // 10 chars + nul FIXME
bool encoder_held = false;
enum {
  MODE_PLAY = 0,
  MODE_CONFIG,
  MODE_COUNT   // how many modes we got
};
int display_mode = MODE_PLAY;

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




//#define MONITOR_CPU  // define to monitor Core 2 CPU usage on pin CPU_USE

#define SAMPLERATE 22050
//#define SAMPLERATE 44100 // VCC-GND 16mb flash boards won't overclock fast enough for 44khz ?

PWMAudio DAC(PWMOUT);  // 16 bit PWM audio

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

int current_track = 0; // track we are working on

// we have 8 voices that can play any sample when triggered
// this structure holds the settings for each voice

#define NUM_VOICES 8
struct voice_t {
  int16_t sample;   // index of the sample structure in sampledefs.h
  int16_t level;   // 0-1000 for legacy reasons
  uint32_t sampleindex; // 20:12 fixed point index into the sample array
  uint16_t sampleincrement; // 1:12 fixed point sample step for pitch changes
  bool isPlaying;  // true when sample is playing
} voice[NUM_VOICES] = {
  0,      // default voice 0 assignment - typically a kick but you can map them any way you want
  850,  // initial level
  0,    // sampleindex
  4096, // initial pitch step - normal pitch
  false, // sample not playing

  1,      // default voice 1 assignment
  850,
  0,    // sampleindex
  4096, // initial pitch step - normal pitch
  false, // sample not playing

  2,    // default voice 2 assignment
  850, // level
  0,    // sampleindex
  4096, // initial pitch step - normal pitch
  false, // sample not playing

  3,    // default voice 3 assignment
  850, // level
  0,    // sampleindex
  4096, // initial pitch step - normal pitch
  false, // sample not playing

  4,    // default voice 4 assignment
  850,  // level
  0,    // sampleindex
  4096, // initial pitch step - normal pitch
  false, // sample not playing

  5,    // default voice 5 assignment
  850,  // level
  0,    // sampleindex
  4096, // initial pitch step - normal pitch
  false, // sample not playing

  6,    // default voice 6 assignment
  850,  // level
  0,    // sampleindex
  4096, // initial pitch step - normal pitch
  false, // sample not playing

  10,    // default voice 7 assignment
  850,   // level
  0,    // sampleindex
  4096, // initial pitch step - normal pitch
  false, // sample not playing
};


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
#include "beatbox/samples.h"

#define NUM_SAMPLES (sizeof(sample)/sizeof(sample_t))

// sample and debounce the keys

#define NUM_BUTTONS 9 // 8 buttons plus USR button on VCC-GND board
#define SHIFT 8 // index of "shift" USR button 
uint8_t debouncecnt[NUM_BUTTONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; // debounce counters
bool button[NUM_BUTTONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; // key active flags
int led[8] = {LED0, LED1, LED2, LED3, LED4, LED5, LED6, LED7};
int buttons[NUM_BUTTONS] = {BUTTON0, BUTTON1, BUTTON2, BUTTON3, BUTTON4, BUTTON5, BUTTON6, BUTTON7, SHIFT};

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

  Serial.begin(115200);

  // Additions
  // ENCODER
  pinMode(encoderA_pin, INPUT_PULLUP);
  pinMode(encoderB_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoderA_pin), checkEncoderPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderB_pin), checkEncoderPosition, CHANGE);
  //encoder_switch.attach(encoderSW_pin, INPUT_PULLUP);
  //encoder_switch.setPressedState(LOW);

  // DISPLAY
  Wire.setSDA(oled_sda_pin);
  Wire.setSCL(oled_scl_pin);
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, oled_i2c_addr)) {
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
  //pinMode(AIN2, INPUT);

  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
  pinMode(LED7, OUTPUT);

  pinMode(CLOCKOUT, OUTPUT);

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

  //seq[0].trigger=0b1000100010001000;
  seq[0].trigger->generateSequence(4, 16);
  display_value(NUM_SAMPLES); // show number of samples on the display

}




// main core handles UI
void loop() {
  bool anybuttonpressed;

  // UI handler

  // ENCODER update picostepseq
  //encoder_switch.update();
  encoder.tick();
  int encoder_pos = encoder.getPosition();
  if ( (encoder_pos != encoder_pos_last )) {
    encoder_delta = encoder_pos - encoder_pos_last;
    if (button[8]) {

      bpm = bpm + encoder_delta;
    }
    displayUpdate();
    display_value(bpm - 50);

  }
  uint32_t now = millis();


  if (encoder_push_millis > 0 ) {
    if (!step_push_millis && (now - encoder_push_millis) > 1000 && !encoder_delta ) {
      if ( !encoder_held ) {
        encoder_held = true;
        display_mode =  (display_mode + 1) % MODE_COUNT;
        if ( display_mode == MODE_PLAY ) { // switched back to play mode
          //configure_sequencer();

        }
      }
    }
    if (step_push_millis > 0) { // we're pushing a step key too
      if (encoder_push_millis < step_push_millis) {  // and encoder was pushed first
        strcpy(seq_info, "saveseq");
      }
    }
  }


  // END picostepseq


  anybuttonpressed = false;
  for (int i = 0; i <= 8; ++i) { // scan all the buttons
    if (button[i]) {
      //displayUpdate();
      anybuttonpressed = true;

        // a track button is pressed
        current_track = i; // keypress selects track we are working on
        //if ((!potlock[1]) || (!potlock[2])) seq[i].trigger=euclid(16,map(potvalue[1],POT_MIN,POT_MAX,0,MAX_SEQ_STEPS),map(potvalue[2],POT_MIN,POT_MAX,0,MAX_SEQ_STEPS-1));

        // look up drum trigger pattern encoder
        if ( (encoder_pos != encoder_pos_last )) {
          rp2040.idleOtherCore();
          int result = voice[i].sample + encoder_delta;
          if (result >= 0 && result <= NUM_SAMPLES - 1) {
            voice[i].sample = result;
          }
          rp2040.resumeOtherCore();
        }
        // change pitch on pot 0
        if (!potlock[0]) { // change sample if pot has moved enough
          voice[current_track].sampleincrement = (uint16_t)(map(potvalue[0], POT_MIN, POT_MAX, 2048, 8192)); // change sample pitch if pot has moved enough
        }

        // set track euclidean triggers if either pot has moved enough
        if (!potlock[1]) {
          seq[i].fills = map(potvalue[1], POT_MIN, POT_MAX, 0, 16);
          seq[i].trigger->generateSequence(seq[i].fills, 15);

          //seq[i].trigger= drumpatterns[map(potvalue[1],POT_MIN,POT_MAX,0,NUMPATTERNS-1)];
        }
        /*
          if (!potlock[2]) {
          seq[i].trigger->setRepeats(map(potvalue[2], POT_MIN, POT_MAX, 0, 32));
          //seq[i].trigger= rightRotate(map(potvalue[2],POT_MIN,POT_MAX,0,MAX_SEQ_STEPS-1),seq[i].trigger,16); // rotate trigger pattern
          }
        */

    }
  }

  /// only set new pos last after buttons have had a chance to use the delta
  encoder_pos_last = encoder_pos;

  // lock pot settings when no keys are pressed so it requires more movement to change value
  // this is so when we change tracks we don't immediately change the settings on the new track
  if (!anybuttonpressed) lockpots();

  // MIDI.read();  // do serial MIDI

  do_clocks();  // process sequencer clocks
  scanbuttons();

  // reading A/D seems to cause noise in the audio so don't do it too often
  if ((millis() - pot_timer) > POT_SAMPLE_TIME) {
    readpot(0);
    readpot(1);
    //readpot(2); // sample pots.
    pot_timer = millis();
  }

  // if display is not busy show track triggers on leds
  if ((millis() - display_timer) > DISPLAY_TIME) {
    //displayUpdate();
    for (int i = 0; i <= 7; ++i) { // LED port numbers are sequential on the Pikocore
      if ( seq[i].trigger->getCurrentStep() ) digitalWrite(led[i], 1);
      else digitalWrite(led[i], 0);
    }
  }
}

// second core setup
// second core dedicated to sample processing
void setup1() {
  delay (1000); // wait for main core to start up perhipherals
}

// second core calculates samples and sends to DAC
void loop1() {
  int32_t newsample, samplesum = 0;
  uint32_t index;
  int16_t samp0, samp1, delta, tracksample;

  // oct 22 2023 resampling code
  // to change pitch we step through the sample by .5 rate for half pitch up to 2 for double pitch
  // sample.sampleindex is a fixed point 20:12 fractional number
  // we step through the sample array by sampleincrement - sampleincrement is treated as a 1 bit integer and a 12 bit fraction
  // for sample lookup sample.sampleindex is converted to a 20 bit integer which limits the max sample size to 2**20 or about 1 million samples, about 45 seconds
  // oct 24/2023 - scan through voices instead of sample array
  // faster because there are only 8 voices vs typically 45 or more samples
  /*
    for (int track=0; track< NTRACKS;++track) {  // look for samples that are playing, scale their volume, and add them up
      tracksample=voice[track].sample; // precompute for a little more speed below
      index=sample[tracksample].sampleindex>>12; // get the integer part of the sample increment
      if (index < sample[tracksample].samplesize) { // if sample is playing, do interpolation
        samp0=sample[tracksample].samplearray[index]; // get the first sample to interpolate
        samp1=sample[tracksample].samplearray[index+1];// get the second sample
        delta=samp1-samp0;
        newsample=(int32_t)samp0+((int32_t)delta*((int32_t)sample[tracksample].sampleindex & 0x0fff))/4096; // interpolate between the two samples
        //samplesum+=((int32_t)samp0+(int32_t)delta*(sample[i].sampleindex & 0x0fff)/4096)*sample[i].play_volume;
        samplesum+=newsample*sample[tracksample].play_volume;
        sample[tracksample].sampleindex+=voice[track].sampleincrement; // add step increment
      }
    }
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
const pos_t trans_text_pos  = {.x = 40, .y = 15, .str = "trs:%+2d" };
const pos_t seqno_text_pos  = {.x = 80, .y = 15, .str = "seq:%d" };
const pos_t seq_info_pos    = {.x = 60, .y = 45, .str = "" };
const pos_t play_text_pos   = {.x = 110, .y = 57, .str = "" };

const pos_t oct_text_offset = { .x = 3, .y = 10,  .str = "" };
const pos_t gate_bar_offset = { .x = 0, .y = -15, .str = "" };
const pos_t edit_text_offset = { .x = 3, .y = 22,  .str = "" };
const int gate_bar_width = 14;
const int gate_bar_height = 4;

void displayUpdate() {
  display.clearDisplay();
  display.setFont(&myfont);
  display.setTextColor(WHITE, 0);
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

  //display.setFont(&myfont2);
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
  display.print("bpm: ");
  display.print(bpm);

  // transpose
  display.setCursor(trans_text_pos.x, trans_text_pos.y);
  display.print("shift: ");
  display.print(button[8]);

  // seqno
  display.setCursor(seqno_text_pos.x, seqno_text_pos.y);
  display.print("delta: ");
  display.print(encoder_delta);  // user sees 1-8

  // seq info / meta
  display.setCursor(seq_info_pos.x, seq_info_pos.y);
  display.print("7: ");
  display.print(button[7]);

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
  // a little LED dance
  /*
    for( int i=0; i<1000; i++) {
    for( int j=0; j<8; j++) {
      int v = 30 + 30 * sin( (j*6.2 / 8 ) + i/50.0 ) ;
      analogWrite( led_pins[j], v);
    }
    delay(1);
    }*/
}