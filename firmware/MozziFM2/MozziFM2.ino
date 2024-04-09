/*
  (c) 2024 blueprint@poetaster.de
  GPLv3

  BASED in part on
  Arduino Mozzi MIDI FM Synthesis 2
  https://diyelectromusic.wordpress.com/2020/08/19/arduino-fm-midi-synthesis-with-mozzi-part-2/

      MIT License
      Copyright (c) 2020 diyelectromusic (Kevin)

  Using principles from the following Arduino tutorials:
    Arduino MIDI Library - https://github.com/FortySevenEffects/arduino_midi_library
    Mozzi Library        - https://sensorium.github.io/Mozzi/

  Much of this code is based on the Mozzi example Knob_LightLevel_x2_FMsynth (C) Tim Barrass
*/
#define AUDIO_CHANNEL_1_PIN 22
#define MOZZI_AUDIO_PIN_1 22

// button inputs
#define BUTTON0  0 // key1 input on schematic
#define BUTTON1  2
#define BUTTON2 4
#define BUTTON3 6
#define BUTTON4 8
#define BUTTON5 10
#define BUTTON6 12
#define BUTTON7 14  // for the Pico board
#define SHIFTBUTTON 28 // was 24 USR button on VCC-GND boards

#define LED0 1 // LED1 output on schematic
#define LED1 3
#define LED2 5
#define LED3 7
#define LED4 9
#define LED5 11
#define LED6 13
#define LED7 15
// analog freq pins

// just in case
#define ARDUINO_ARCH_RP2040 1

#include <MIDI.h>
#include <Mozzi.h>
#include <Oscil.h> // oscillator
#include <tables/cos2048_int8.h> // table for Oscils to play
#include <mozzi_midi.h>
#include <Smooth.h>
#include <AutoMap.h> // maps unpredictable inputs to a range
#include <ADSR.h>
#include <Wire.h>

#include <RotaryEncoder.h>
//#include <Bounce2.h>
#include <Adafruit_SSD1306.h>

#include "font.h"
#include "helvnCB6pt7b.h"
#define myfont helvnCB6pt7b // Org_01 looks better but is small.
const int encoderA_pin = 19;
const int encoderB_pin = 18;
const int encoderSW_pin = 28;
const int oled_sda_pin = 20;
const int oled_scl_pin = 21;
const int oled_i2c_addr = 0x3C;
const int dw = 128;
const int dh = 64;
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(dw, dh, &Wire, OLED_RESET);

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
bool encoder_held = false;

// the somewhat flake code from my pikobeats version

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
enum {
  MODE_PLAY = 0,
  MODE_CONFIG,
  MODE_COUNT   // how many modes we got
};

int display_mode = 0;
uint8_t display_repeats = 0;
// flag all pot values as locked ie they have to change more than MIN_POT_CHANGE to register
void lockpots(void) {
  for (int i = 0; i < NPOTS; ++i) potlock[i] = 1;
}

const int INTS_PIN = 26; // set the analog input for fm_intensity
const int RATE_PIN = 2; // set the analog input for mod rate
const int MODR_PIN = 27; // set the analog input for mod ratio

// sample analog pot input and do filtering.
// if pots are locked, change value only if movement is greater than MIN_POT_CHANGE
uint16_t readpot(uint8_t potnum) {
  int val = 0;
  int input;
  switch (potnum) { // map potnum to RP2040 pin
    case 0:
      input = INTS_PIN;
      break;
    case 1:
      input = RATE_PIN;
      break;
    case 2:
    default:   // shouldn't happen
      input = INTS_PIN;
      break;
  }
  // note that Pikocore pots are wired "backwards" - max voltage is full ccw
  for (int j = 0; j < POT_AVERAGING; ++j) val += (1024 - mozziAnalogRead(input)); // read the A/D a few times and average for a more stable value
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
// largely defunct defines

int current_track;
int bpm = 120;

uint16_t noteA;
uint16_t noteM;
uint16_t attackIn = 8; // up to 384
uint16_t durationIn = 200; // up to 384
uint16_t modIn = 200; // up to 2800
uint16_t modC = 0;
int pressedB;
int RATE_value = 1 ; //was an adc in kevin's orig

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
// display functions
typedef struct {
  int x;
  int y;
  const char* str;
} pos_t;

//// {x,y} locations of play screen items
const int step_text_pos[] = { 0, 15, 16, 15, 32, 15, 48, 15, 64, 15, 80, 15, 96, 15, 112, 15 };
const pos_t bpm_text_pos    = {.x = 0,  .y = 15, .str = "bpm:%3d" };
const pos_t trans_text_pos  = {.x = 46, .y = 15, .str = "trs:%+2d" };
const pos_t seqno_text_pos  = {.x = 90, .y = 15, .str = "seq:%d" };
const pos_t seq_info_pos    = {.x = 0, .y = 35, .str = "" };
const pos_t mode_text_pos   = {.x = 0, .y = 55, .str = "" };
const pos_t play_text_pos   = {.x = 90, .y = 55, .str = "" };

const pos_t oct_text_offset = { .x = 3, .y = 10,  .str = "" };
const pos_t gate_bar_offset = { .x = 0, .y = -15, .str = "" };
const pos_t edit_text_offset = { .x = 3, .y = 22,  .str = "" };
const int gate_bar_width = 14;
const int gate_bar_height = 4;


bool debugging = true;
float freqs[12] = { 261.63f, 277.18f, 293.66f, 311.13f, 329.63f, 349.23f, 369.99f, 392.00f, 415.30f, 440.00f, 466.16f, 493.88f};


MIDI_CREATE_DEFAULT_INSTANCE();

AutoMap kMapIntensity(0, 4095, 10, 800);
AutoMap kMapModSpeed(0, 4095, 10, 10000);


Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCarrier(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModulator(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kIntensityMod(COS2048_DATA);

int mod_ratio = 5; // brightness (harmonics)
long fm_intensity; // carries control info from updateControl to updateAudio

// smoothing for intensity to remove clicks on transitions
float smoothness = 0.95f;
Smooth <long> aSmoothIntensity(smoothness);

int carrier_freq;

// envelope generator
ADSR <CONTROL_RATE, AUDIO_RATE> envelope;

#define LED 15 // shows if MIDI is being recieved

void HandleNoteOn(byte channel, byte note, byte velocity) {
  if (velocity == 0) {
    HandleNoteOff(channel, note, velocity);
    return;
  }
  carrier_freq = mtof(note);
  setFreqs();
  envelope.noteOn();
  digitalWrite(LED, HIGH);
}

void aNoteOn(float note, int velocity) {
  if (velocity == 0) {
    aNoteOff(note, velocity);
    return;
  }
  carrier_freq = note;
  setFreqs();
  envelope.noteOn();
  //digitalWrite(LED, HIGH);
}

void HandleNoteOff(byte channel, byte note, byte velocity) {
  envelope.noteOff();
  digitalWrite(LED, LOW);
}

void aNoteOff( float note, int velocity) {
  envelope.noteOff();
  //digitalWrite(LED, LOW);
}

void setup() {
  if (debugging) {
    Serial.begin(9600);
    Serial.println(F("YUP"));
  }
  // Additions
  // ENCODER
  pinMode(encoderA_pin, INPUT_PULLUP);
  pinMode(encoderB_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoderA_pin), checkEncoderPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderB_pin), checkEncoderPosition, CHANGE);
  // DISPLAY
  Wire.setSDA(oled_sda_pin);
  Wire.setSCL(oled_scl_pin);
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, oled_i2c_addr)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;) ;  // Don't proceed, loop forever
  }
  envelope.setADLevels(255, 200);
  envelope.setTimes(50, 200, 1000, 200); // 10000 is so the note will sustain 10 seconds unless a noteOff comes

  startMozzi();
  
  displaySplash();
  
  pinMode(BUTTON0, INPUT_PULLUP);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);
  pinMode(BUTTON5, INPUT_PULLUP);
  pinMode(BUTTON6, INPUT_PULLUP);
  pinMode(BUTTON7, INPUT_PULLUP);
  pinMode(SHIFTBUTTON, INPUT_PULLUP);

  pinMode(26, INPUT);
  pinMode(27, INPUT);

  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
  pinMode(LED7, OUTPUT);

  pinMode(LED, OUTPUT);
  
  pinMode(23, OUTPUT); // thi is to switch to PWM for power to avoid ripple noise
  digitalWrite(23, HIGH);

  
  // Connect the HandleNoteOn function to the library, so it is called upon reception of a NoteOn.
  //MIDI.setHandleNoteOn(HandleNoteOn);  // Put only the name of the function
  //MIDI.setHandleNoteOff(HandleNoteOff);  // Put only the name of the function
  //MIDI.begin(MIDI_CHANNEL_OMNI);


}

void setFreqs() {
  //calculate the modulation frequency to stay in ratio
  int mod_freq = carrier_freq * mod_ratio;
  // set the FM oscillator frequencies
  aCarrier.setFreq(carrier_freq);
  aModulator.setFreq(mod_freq);
}


void updateControl() {
  //MIDI.read();

  envelope.update();

  mod_ratio = mozziAnalogRead(MODR_PIN) >> 7; // value is 0-7

  setFreqs();

  int INTS_value = mozziAnalogRead(INTS_PIN); // value is 0-1023
  int INTS_calibrated = kMapIntensity(INTS_value);

  // calculate the fm_intensity
  fm_intensity = ((long)INTS_calibrated * (kIntensityMod.next() + 128)) >> 8; // shift back to range after 8 bit multiply

  //int RATE_value = mozziAnalogRead(RATE_PIN); // value is 0 - 4095
  // use a float here for low frequencies
  float mod_speed = (float)kMapModSpeed(RATE_value) / 10;
  
  //float mod_speed = (float) RATE_value ;
  kIntensityMod.setFreq(mod_speed);
}

AudioOutput_t  updateAudio() {
  long modulation = aSmoothIntensity.next(fm_intensity) * aModulator.next();
  return (int)((envelope.next() * aCarrier.phMod(modulation)) >> 8);
}


void loop() {
  audioHook();
}

// second core dedicated to display foo

void setup1() {
  delay (1000); // wait for main core to start up perhipherals
}

// second core calculates samples and sends to DAC
void loop1() {
  uint32_t now = millis();
  bool anybuttonpressed;
  anybuttonpressed = false;

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
        if ( display_mode == 0 ) { // switched back to play mode
          display_mode = 1;
          //configure_sequencer();
        } else {
          display_mode = 0;
        }
      }
    }

    if (step_push_millis > 0) { // we're pushing a step key too
      if (encoder_push_millis < step_push_millis) {  // and encoder was pushed first
        //strcpy(seq_info, "saveseq");
      }
    }
  }

  

  for (int i = 0; i < 9; ++i) { // scan all the buttons
    if (button[i]) {

      anybuttonpressed = true;
      if (i < 8)  digitalWrite(led[i] , HIGH);
      
      // a track button is pressed
      current_track = i; // keypress selects track we are working on
      
      if ( encoder_delta == 0) {
        //if (pressedB != i) {
          // turn off the last note
          //aNoteOff(freqs[pressedB],0);
        //}
        aNoteOff(freqs[pressedB],0);
        noteA = freqs[i];
        aNoteOn( freqs[i], 100 );
      } 
      pressedB = i;
      
      //  if ((!potlock[1]) || (!potlock[2])) seq[i].trigger=euclid(16,map(potvalue[1],POT_MIN,POT_MAX,0,MAX_SEQ_STEPS),map(potvalue[2],POT_MIN,POT_MAX,0,MAX_SEQ_STEPS-1));
      // look up drum trigger pattern encoder play modes
      
      if ( i == 2 && encoder_delta != 0 ) {
        attackIn += encoder_delta * 5;
        envelope.setAttackTime(attackIn);
        //attackIn = constrain(attackIn, 0, 380);
      }
      if ( i == 3 && encoder_delta != 0 ) {
        durationIn += encoder_delta * 5;
        envelope.setDecayTime(durationIn);
        //durationIn= constrain(durationIn, 16, 380);
        //pressedB = 1;
      }
      if ( i == 4 && encoder_delta != 0) {
        modIn += encoder_delta * 10;
        modIn = constrain(modIn, 10, 2800);
      }
      
      if ( (encoder_pos != encoder_pos_last ) && display_mode == 1 ) {
        //uint8_t re = seq[i].trigger->getRepeats() + encoder_delta;
        //seq[i].trigger->setRepeats(encoder_delta);
        //display_repeats = seq[i].trigger->getRepeats();

      }

      // change pitch on pot 0
      if (display_mode == 0 ) { // change sample if pot has moved enough
        //noteA = (map(mozziAnalogRead(MODR_PIN), POT_MIN, POT_MAX, 200, 10000));
        //voice[current_track].sampleincrement = (uint16_t)(map(potvalue[0], POT_MIN, POT_MAX, 2048, 8192)); // change sample pitch if pot has moved enough
      }

      // change volume on pot 1
      if (display_mode == 0) {
        //mod_to_carrier_ratio = (map(mozziAnalogRead(AIN1), POT_MIN, POT_MAX, 1, 20));
        //voice[current_track].level = (int16_t)(map(potvalue[1], POT_MIN, POT_MAX, 0, 1000));
        // change sample volume level if pot has moved enough
      }

      if (!potlock[0] && display_mode == 1 ) {
        //filter_fc = potvalue[0] * (LPF_MAX + 10) / 4096;
      }

      // set track euclidean triggers if either pot has moved enough
      if (!potlock[1] && ! button[8] && display_mode == 1) {
        //  seq[i].fills = map(potvalue[1], POT_MIN, POT_MAX, 0, 16);
        //  seq[i].trigger->generateSequence(seq[i].fills, 15);
        //seq[i].trigger= drumpatterns[map(potvalue[1],POT_MIN,POT_MAX,0,NUMPATTERNS-1)];
      }

    } else {
      if (i < 8) digitalWrite(led[i] , LOW); // else the button is off.
    }
  }

  // now, after buttons check if only encoder moved and no buttons
  // this is broken by mozzi, sigh.
  if (! anybuttonpressed && encoder_delta) {
    //bpm = bpm + encoder_delta;
    RATE_value = RATE_value + encoder_delta;
    //display_value(RATE_value - 50); // this is wrong, bro :)
  }


  /// only set new pos last after buttons have had a chance to use the delta
  encoder_pos_last = encoder_pos;
  encoder_delta = 0;  // we've used it

  // start tracking time encoder button held
  if ( ! digitalRead( SHIFTBUTTON ) ) {
    encoder_push_millis = now;
  } else {
    encoder_push_millis = 0;
    encoder_held = false;
  }

  // lock pot settings when no keys are pressed so it requires more movement to change value
  // this is so when we change tracks we don't immediately change the settings on the new track
  if (!anybuttonpressed) lockpots();

  // reading A/D seems to cause noise in the audio so don't do it too often
  if ((now - pot_timer) > POT_SAMPLE_TIME) {
    readpot(0);
    readpot(1);
    pot_timer = now;
  }
  scanbuttons();
  displayUpdate();

}
// display functions
void displayUpdate() {
  display.clearDisplay();
  //display.setFont(&myfont); don't need to call this every time!
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
    const pos_t bpm_text_pos    = {.x=0,  .y=57, .str="bpm:%3d" };
    const pos_t trans_text_pos  = {.x=55, .y=57, .str="trs:%+2d" };
    const pos_t seqno_text_pos  = {.x=0,  .y=45, .str="seq:%d" };
    const pos_t seq_info_pos    = {.x=60, .y=45, .str="" };
    const pos_t play_text_pos   = {.x=110,.y=57, .str="" };
  */
  // bpm
  display.setCursor(bpm_text_pos.x, bpm_text_pos.y);
  display.print("r: ");
  display.print(RATE_value);

  // transpose
  display.setCursor(trans_text_pos.x, trans_text_pos.y);
  display.print("B: ");
  display.print(mod_ratio);

  // seqno
  display.setCursor(seqno_text_pos.x, seqno_text_pos.y);
  display.print("mod: ");
  display.print(modIn);  // user sees 1-8

  // seq info / meta
  display.setCursor(seq_info_pos.x, seq_info_pos.y);
  display.print("att: ");
  display.print(attackIn);
  display.setCursor(mode_text_pos.x, play_text_pos.y);
  display.print("dur: ");
  display.print(durationIn);
  // play/pause
  display.setCursor(play_text_pos.x, play_text_pos.y);
  display.print("mode: ");
  display.print(pressedB);

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
