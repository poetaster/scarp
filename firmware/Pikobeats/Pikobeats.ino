/* Copyright 2023 Rich Heslip, 2024-2025 Mark Washeim

  Author: Rich Heslip (ORIGINAL AUTHOR)
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

#include <EEPROM.h>
#include <PWMAudio.h>
#include <Wire.h>
#include <RotaryEncoder.h>

#include "io.h"
#include "euclid.h"

//#include "filter.h"

bool debug = false;
uint8_t device_initialized = 0;

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

#define DISPLAY_TIME 2000 // time in ms to display numbers on LEDS
int32_t display_timer;


// rotate trigger pattern
uint16_t rightRotate(int shift, uint16_t value, uint8_t pattern_length) {
  uint16_t mask = ((1 << pattern_length) - 1);
  value &= mask;
  return ((value >> shift) | (value << (pattern_length - shift))) & mask;
}

// clock timer we use for clk input and encoder input bpm setting
#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     4
#include "RPi_Pico_TimerInterrupt.h"
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
volatile int clk_display;
uint32_t clk_sync_last;



bool TimerHandler0(struct repeating_timer *t)
{
  (void) t;

  if ( digitalRead(CLOCKIN) && clk_state_last != digitalRead(CLOCKIN)) {
    //min time between pulses has passed
    // calculate bpm
    RPM = (float) ( 60000.0f / ( rotationTime * TIMER0_INTERVAL_MS ) / 2.0f );
    clk_display = RPM;
    // these are for the sequencer
    sync = true;
    rotationTime = 0;
    debounceCounter = 0;
  } else {
    debounceCounter++;
  }

  if (rotationTime >= 1000) {
    // If idle, set RPM to 0, don't increase rotationTime
    sync = false ;// flag for seq.h
    RPM = 0;
    rotationTime = 0;
  } else {
    rotationTime++;
  }

  clk_state_last = digitalRead(CLOCKIN);
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
uint8_t selected_preset = 0;
uint8_t selected_slot   = 0; // saving to

// the display is offset + 1
uint8_t display_preset = 0;
uint8_t display_slot = 0;

uint8_t loadSave = 0;
bool loading = false;

// currently not used
int step_push = -1;
bool step_edited = false;
char seq_info[11];  // 10 chars + nul FIXME

bool encoder_held = false;

// pots & buttons settings and functions
#include "pots.h"


enum {
  MODE_PLAY = 0,
  MODE_CONFIG,
  MODE_COUNT   // how many modes we got
};

int display_mode = MODE_PLAY;
uint8_t display_repeats = 0;
uint8_t display_fills = 0;
uint8_t display_rand = 0;
uint8_t display_samp = 0;
uint8_t display_vol = 175;
uint8_t display_pitch = 128;
String display_pat;

int modulation_timer = 0;
int rotation_timer = 0;
uint8_t last_mod_chan = 0;
uint8_t last_rot_chan = 0;

// from pikocore filter NOT used currently
/*
  uint8_t filter_fc = LPF_MAX + 10;
  uint8_t hpf_fc = 0;
  uint8_t filter_q = 0;
*/

//#define MONITOR_CPU  // define to monitor Core 2 CPU usage on pin CPU_USE

// setup audio hardware

//#define SAMPLERATE 22050
#define SAMPLERATE 44100 // VCC-GND 16mb flash boards won't overclock fast enough for 44khz ?

PWMAudio DAC(PWMOUT);  // 16 bit PWM audio



/* we have 8 voices that can play any sample when triggered
  this structure holds the settings for each voice
  each voice has 4 samples assigned to it for a total of 32
  we use a header per sample set
  we can have an arbitrary number of samples but you will run out of memory at some point

  sound sample files are 22khz 16 bit signed PCM format - see the sample include files for examples
  you can change the sample rate to whatever you want but most testing was done at 22khz. 44khz probably works but not much testing was done
  use the wave2header22khz.exe utility to automagically batch convert all the .wav files in a directory into the required header files
  put your 22khz or 44khz PCM wav files in a sample subdirectory with a copy of the utility, run the utility and it will generate all the required header files
  wave2header creates a header file containing the signed PCM values for each sample - note that it may change the name of the file if required to make it "c friendly"
  wave2header also creates sampledefs.h which is an array of structures containing information about each sample file
  the samples are arranged in alphabetical order to facilitate grouping samples by name - you can manually edit this file to change the order of the samples as needed
  sampledefs.h contains other information not used by this program e.g. the name of the sample file - I wrote it for another project
   wave2header also creates "samples.h" which #includes all the generated header files
*/


//#include "80s.h"
//#include "angularj.h"
#include "mixp.h"
//#include "tekke.h"

//#include "beatbox.h"
//#include "bbox.h"

// pico 2 only, these are 44khz since there is enough space on the pico2
//#include "mix.h"

#define NUM_SAMPLES (sizeof(sample)/sizeof(sample_t))


// include here to avoid forward references - I'm lazy :)
#include "seq.h"
#include "eeprom.h"
#include "display.h"


bool starting = false;

// main core setup
void setup() {
  // set clock speed as in picokore
  //set_sys_clock_khz(264000, true); don't do this :)

  Serial.begin(115200);

  if (debug) {
    Serial.print(F("\nStarting RPM_Measure on ")); Serial.println(BOARD_NAME);
    Serial.println(RPI_PICO_TIMER_INTERRUPT_VERSION);
    Serial.print(F("CPU Frequency = ")); Serial.print(F_CPU / 1000000); Serial.println(F(" MHz"));
  }
  // Interval in microsecs
  if (ITimer0.attachInterruptInterval(TIMER0_INTERVAL_MS * 1000, TimerHandler0))
  {
    Serial.print(F("Starting  ITimer0 OK, millis() = ")); Serial.println(millis());
  }  else {
    Serial.println(F("Can't set ITimer0. Select another freq. or timer"));
  }
  //Serial.flush();

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

  display_value(NUM_SAMPLES); // show number of samples on the display

  // try to retrieve saved preset if not, init slots
  EEPROM.begin(2048); // the 8 slots we save now are only 384 bytes, but it'll probably grow
  delay(50);

  starting = true;



  // we read the first byte to try to get a preset, stored 0-7
  if (starting) {
    loading = true;
    loadInit();
    if (device_initialized == 7 ) {

      loadLastPreset(); // sets selected_preset from base eeprom save point
      loadMemorySlots();
      if (selected_preset > -1 && selected_preset < 8) {

        loadFromMemorySlot(selected_preset);

      } else {
        loadFromMemorySlot(0);
      }

    } else {
      // first use of device, so prep it.
      initializeEEPROM(); // first run, get settings into all 8 slots
      currentConfig = defaultSlots[0]; // current config is default 0
      bpm = currentConfig.tempo;
      internalClock = currentConfig.internalClock;
      selected_preset = 0;
      saveCurrentPreset(selected_preset); // save to eeprom

      updateRythm(); // update the sequencers

      writeInit(); // set flag so we don't repeat
    }

    starting = false;
    loading = false;
  }

  // now start the dac
  // set up Pico PWM audio output
  DAC.setBuffers(4, 128); // DMA buffers
  DAC.begin(SAMPLERATE);
  delay(100);
  modulation_timer = millis();
  rotation_timer = millis();
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

    // first mode change on encoder button push, short
    if ((now - encoder_push_millis) < 100 && ! encoder_delta ) {
      if ( !encoder_held ) {
        encoder_held = true;
        display_mode = display_mode + 1;
        if ( display_mode > 3) { // switched back to play mode
          display_mode = 0;
          //configure_sequencer();
        }
      }
    } else if ( (now - encoder_push_millis) > 100 && ! encoder_delta && display_mode == 3 ) {
      // or change  between load and save preset
      //loadSave = !loadSave ; // toggle between load and save
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

      // use encoder delta with selected button to set sample for track
      if ( (encoder_pos != encoder_pos_last ) && ! button[8] && display_mode == 0) {
        voice[i].isPlaying = false;
        // here we rotate at offset since we're now arranging sample sets in groups of 4
        // currently only really works with 'mix.h'
        int result ;
        if (encoder_delta > 0) {
          result = voice[current_track].sample + 8 ;
        } else {
          result = voice[current_track].sample - 8 ;
        }
        if (result >= 0 && result <= NUM_SAMPLES - 1) {
          voice[current_track].sample = result;
          currentConfig.sample[i] = result; // save config 0 - 31

        }
      }

      // use encoder button with selected button to set the offset
      if ( (encoder_pos != encoder_pos_last ) && display_mode == 1 ) {
        int repeats = seq[current_track].repeats;
        repeats = constrain( (repeats + encoder_delta), 0, 5);
        seq[i].repeats = repeats;

        // repeats are actually now OFFSET. the repeats mechanism with rotate
        // was disturbing

        display_repeats = repeats;
        seq[i].trigger->rotate(repeats);
        currentConfig.offset[i] = repeats;

        // if offset is 0, reset
        if (repeats == 0 && currentConfig.randy[i] == 0) {
          seq[i].trigger->generateSequence(seq[i].fills, 16);
        } else if (repeats == 0 && currentConfig.randy[i] == 1) {
          seq[i].trigger->generateRandomSequence(seq[i].fills, 16);
        }

      }
      // either restrieve or save preset
      if ( display_mode == 3 &&  ! button[8] ) {
        if (loadSave == 0 && ! loading ) {
          loading = true;
          currentConfig.tempo = bpm;
          updateMemorySlot(selected_preset); // update the memory slot before switching
          selected_preset = current_track;
          //DAC.end();
          saveToEEPROM(current_track);
          //DAC.begin();
          loading = false;

        } else if (loadSave == 1 && ! loading && selected_preset != current_track) {
          loading = true; // make sure audio is off
          currentConfig.tempo = bpm;
          updateMemorySlot(selected_preset); // update the memory slot before switching
          selected_preset = current_track; // set selected preset
          loadFromMemorySlot(current_track); // load it from memory
          loading = false;
        }
      }

      // change pitch on pot 0
      if (!potlock[0] && ( display_mode == 0 || display_mode == 2) ) { // change sample if pot has moved enough
        uint16_t pitch = (uint16_t)(map(potvalue[0], POT_MIN, POT_MAX, 2048, 8160));
        // divisible by 2 and it won't click
        if (pitch % 2 == 0) {
          voice[current_track].sampleincrement = pitch;  // change sample pitch if pot has moved enough
          display_pitch = constrain( (pitch >> 5), 1, 255) ; // show 8 bits, which we also store
          currentConfig.pitch[i] = display_pitch; // update config for this channel
        }
      }

      // change sample volume level if pot has moved enough
      if (!potlock[1] && display_mode == 0) {
        int16_t level = (int16_t)(map(potvalue[1], POT_MIN, POT_MAX, 0, 1023));
        voice[current_track].level = level;
        display_vol = constrain( ( level >> 2 ), 1, 255); // show 8 bits which we store
        currentConfig.volume[i] = display_vol;

      }
      if (!potlock[0] && display_mode == 1 ) {
        // set track euclidean triggers, random, if either pot has moved enough
        //filter_fc = potvalue[0] * (LPF_MAX + 10) / 4096;
        seq[i].fills = map(potvalue[0], POT_MIN, POT_MAX, 0, 16);
        seq[i].trigger->generateRandomSequence(seq[i].fills, 16);
        currentConfig.randy[i] = 1;
        currentConfig.fills[i] = seq[i].fills;

      }

      // set track euclidean triggers if either pot has moved enough
      if (!potlock[1] && ! button[8] && display_mode == 1) {
        seq[i].fills = map(potvalue[1], POT_MIN, POT_MAX, 0, 16);
        seq[i].trigger->generateSequence(seq[i].fills, 16);
        seq[i].trigger->resetSequence(); // set to 0
        currentConfig.randy[i] = 0;
        currentConfig.fills[i] = seq[i].fills;

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
    // update display values on and update config object with changes
    // do not update values on load save screen
    if (current_track > -1 && current_track < 8 && display_mode != 3) {
      display_pat = String(seq[current_track].trigger->textSequence);
      display_pitch = currentConfig.pitch[current_track] ;
      display_vol = currentConfig.volume[current_track] ;
      display_repeats = seq[current_track].repeats;
      display_fills = seq[current_track].fills;
      display_rand = currentConfig.randy[current_track];
      display_samp = voice[current_track].sample;
      currentConfig.tempo = bpm;
      updateMemorySlot(selected_preset); // update the memory slot before switching
    }
  }



  // now, after buttons check if only encoder moved and no buttons
  if (! anybuttonpressed && encoder_delta && display_mode != 3) {
    bpm = bpm + encoder_delta;
    currentConfig.tempo = bpm;
    displayUpdate();
    display_value(bpm - 50);
  } else if (! anybuttonpressed && encoder_delta && display_mode == 3) {
    // in load save mode, switch between load and save
    if (encoder_delta > 0) {
      loadSave = 0 ;
    } else {
      loadSave = 1;
    }
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

  scanbuttons();

  // reading A/D seems to cause noise in the audio so don't do it too often
  if ((now - pot_timer) > POT_SAMPLE_TIME) {
    readpot(0);
    readpot(1);
    pot_timer = now;
  }

  // do random modulation on volume
  // this is not so nice although it does work
  // it should be a gradual change over time,
  // not a hard cut
/*
  if ( (now - modulation_timer ) > 50 ) {

    // set a new channel
    int chan = constrain ( (random(8) - 1) , 0, 7) ;

    // don't do this if the channels are playing
    if (  voice[chan].isPlaying == false && voice[last_mod_chan].isPlaying == false ) {

      // restore the configured volume of last modified
      voice[last_mod_chan].level = ( currentConfig.volume[last_mod_chan] << 2);

      // set a new level based on previous
      int level = constrain( random(voice[chan].level) , 300, voice[chan].level);
      voice[chan].level = level;
      last_mod_chan = chan;

    }

    modulation_timer = now;

  }
  */


  // do shift of a channel offset every 2 seconds, but don't do this if we're modifing the settings.
  /*
    if ( (now - rotation_timer ) > 5000 && display_mode != 1 ) {

      int chan = constrain ( (random(8) - 1) , 0, 7) ;
      int rep = constrain ( (random(4) - 1) , 0, 3) ;

      loading = true; // avoid advancing head
      if ( voice[chan].isPlaying == false ) {
        seq[chan].trigger->resetSequence();

        last_rot_chan = chan;
      }
      loading = false;
      rotation_timer = now; // reset timer
    }
  */

  // if display is not busy show track triggers on leds
  if ((now - display_timer) > DISPLAY_TIME) {

    if (display_mode == 3 ) {
      displayLoadSaveUpdate();
    } else {
      displayUpdate();
    }

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

// this is duplicating the timer work at the beginning. sigh.
bool current_clk = false;
bool last_clk = false;
long clk_count = 0;

// second core calculates samples and sends to DAC
void loop1() {

   bool externalSync;
  // we're just counting state transitions and every second pulse is
  // a clock tick. if there is not  input see the RPM ....
  current_clk = digitalRead(CLOCKIN);
  
  if ( current_clk != last_clk ) {
    clk_count++;
    if ( clk_count % 2 == 0) {
      // and play
      reset = true;
      do_clocks();
      // sync out
      digitalWrite(CLOCKOUT, 1);
    }
  }
  last_clk = current_clk;


  // we have input which we measure the time of or not. if not, do internal clocks.
  // this is crappy, but works :)
  if ( RPM > bpm + 1 || RPM < bpm - 1 && RPM > 49) {
    //reset = true; //reset seq
    bpm = RPM;
    externalSync = true;
  } else {
    // use internal clock if no input
    externalSync = false;
    do_clocks();
  }

  if (! loading ) {

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

}
