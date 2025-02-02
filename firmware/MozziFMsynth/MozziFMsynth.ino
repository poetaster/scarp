/*  Example of simple FM with the phase modulation technique,
    using Mozzi sonification library.

    Demonstrates Oscil::phMod() for phase modulation,
    Smooth() for smoothing control signals,
    and Mozzi's fixed point number types for fractional frequencies.

    Also shows the limitations of Mozzi's 16384Hz Sample rate,
    as aliasing audibly intrudes as the sound gets brighter around
    midi note 48.

    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.io/Mozzi/

		Mozzi documentation/API
		https://sensorium.github.io/Mozzi/doc/html/index.html

		Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2012, CC by-nc-sa.
*/
#define AUDIO_CHANNEL_1_PIN 22
#define AUDIO_CHANNEL_2_PIN 16
#define MOZZI_AUDIO_PIN_1 22
#define MOZZI_AUDIO_PIN_2 16
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
#define AIN0 26
#define AIN1 27
//#define CENTREFREQ_PIN 2

// modulation pins analog
//#define M1P 5
//#define M2P 6
//#define M3P 7
//#define FLT_PIN 3

#define ARDUINO_ARCH_RP2040 1
#include <Mozzi.h>
#include <Oscil.h>
#include <tables/cos2048_int8.h> // table for Oscils to play
#include <mozzi_midi.h>
#include <mozzi_fixmath.h>
#include <EventDelay.h>
#include <Smooth.h>
#include <Ead.h>
#include <ADSR.h>
#include <Smooth.h>
#include <AutoMap.h> // maps unpredictable inputs to a range
#include <ADSR.h>

//MIDI_CREATE_DEFAULT_INSTANCE();

AutoMap kMapIntensity(0,4096,10,700);
AutoMap kMapModSpeed(0,4096,10,10000);



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

//Adafruit_SSD1306 display(dw, dh, &Wire, -1);
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
      //if (debouncecnt[i] <= 3) ++debouncecnt[i];
      //if (debouncecnt[i] == 2) { // trigger on second sample of key active
        button[i] = 1;
      //}
    }
    else {
      //debouncecnt[i] = 0;
      button[i] = 0;
    }
  }
  if (pressed) return true;
  else return false;
}

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


#define CONTROL_RATE 256 // Hz, powers of 2 are most reliable

Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCarrier(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModulator(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kModIndex(COS2048_DATA);

// The ratio of deviation to modulation frequency is called the "index of modulation". ( I = d / Fm )
// It will vary according to the frequency that is modulating the carrier and the amount of deviation.
// so deviation d = I   Fm
// haven't quite worked this out properly yet...

Q8n8 mod_index;// = float_to_Q8n8(2.0f); // constant version
Q16n16 deviation;

Q16n16 carrier_freq, mod_freq;

// FM ratio between oscillator frequencies, stays the same through note range
Q8n8 mod_to_carrier_ratio = float_to_Q8n8(3.f);

int mod_ratio = 5; // brightness (harmonics)
long fm_intensity;
EventDelay kNoteChangeDelay;

ADSR <CONTROL_RATE, AUDIO_RATE> envelope;
//byte attack_level = rand(128)+127;
//byte decay_level = rand(255);
unsigned int sustain, decay, release_ms, gain;

// for note changes
Q7n8 target_note, last_note, note0, note1, note_upper_limit, note_lower_limit, note_change_step, smoothed_note;
Q8n0 octave_start_note = 42;

// using Smooth on midi notes rather than frequency,
// because fractional frequencies need larger types than Smooth can handle
// Inefficient, but...until there is a better Smooth....

Smooth <int> kSmoothNote(0.95f);

void setup(){
    Serial.begin(9600); 
    Serial.println(F("YUP"));
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

  displaySplash();
  //displayConfig();

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
  
  kNoteChangeDelay.set(768); // ms countdown, taylored to resolution of CONTROL_RATE
  kModIndex.setFreq(.768f); // sync with kNoteChangeDelay
  
  envelope.setADLevels(255, 64);
  envelope.setTimes(50, 200, 10000, 200); // 10000 is so the note will sustain 10 seconds unless a noteOff comes
  
  target_note = note0;
  note_change_step = Q7n0_to_Q7n8(3);
  note_upper_limit = Q7n0_to_Q7n8(50);
  note_lower_limit = Q7n0_to_Q7n8(32);
  note0 = note_lower_limit;
  note1 = note_lower_limit + Q7n0_to_Q7n8(5);
  startMozzi(CONTROL_RATE);
}

Q8n8 ratio;
void setFreqs(Q8n8 midi_note){
  carrier_freq = Q16n16_mtof(Q8n8_to_Q16n16(midi_note)); // convert midi note to fractional frequency
  mod_freq = ((carrier_freq >>8) * mod_to_carrier_ratio)  ; // (Q16n16>>8)   Q8n8 = Q16n16, beware of overflow
  deviation = ((mod_freq>>16) * mod_index); // (Q16n16>>16)   Q8n8 = Q24n8, beware of overflow
  aCarrier.setFreq_Q16n16(carrier_freq);
  aModulator.setFreq_Q16n16(mod_freq);
}

int current_track;
int bpm = 120;

uint16_t noteA;
uint16_t noteM;
uint16_t attackIn = 8; // up to 384
uint16_t durationIn = 200; // up to 384
uint16_t modIn = 200; // up to 2800
uint16_t modC = 0;
int pressedB;

void updateControl(){

  updateFM();

}




void updateFM() {
  
  
  int target_note = noteA;
  if (kNoteChangeDelay.ready()) {
    
        // change octave now and then
    if(random((byte)5)==0){
      last_note = 36+(random((byte)6)*12);
    }

    // change step up or down a semitone occasionally
    if(random((byte)13)==0){
      last_note += 1-random((byte)3);
    }

        // change direction
    if(note0>note_upper_limit) note_change_step = Q7n0_to_Q7n8(-3);
    if(note0<note_lower_limit) note_change_step = Q7n0_to_Q7n8(3);
    
     if (target_note==note0){
      note1 += note_change_step;
      target_note=note1;
    }
    else{ 
      note0 += note_change_step;
      target_note=note0;
    }

    decay = durationIn - attackIn; //map(new_value,1,255,64,256) ;
    envelope.setADLevels(attackIn, decay);
    envelope.setTimes(attackIn,200,durationIn,500); 
    // reset eventdelay
    envelope.noteOn();
    kNoteChangeDelay.start(attackIn+durationIn+700);
  }
/*
  int INTS_value= modIn; // value is 0-1023
  int INTS_calibrated = kMapIntensity(INTS_value);
  
 // calculate the fm_intensity
  fm_intensity = ((long)INTS_calibrated * (kModIndex.next()+128))>>8; // shift back to range after 8 bit multiply
  */
  
  last_note = target_note;

  //int modulate = ( mozziAnalogRead(BANDWIDTH_PIN)  + mozziAnalogRead(M2P) ) / 2;
  //int modI = map(modulate, 0, 1023, 200, 2800);

  // vary the modulation index
  mod_index = (Q8n8)modIn + kModIndex.next();

  smoothed_note = kSmoothNote.next(target_note);
  setFreqs(target_note);


}

AudioOutput_t updateAudio(){
    Q15n16 modulation = deviation * aModulator.next() >> 8;
    return (envelope.next() * (int)aCarrier.phMod(modulation)) >> 8;
  //int modulation = kSmoothNote.next(fm_intensity) * aModulator.next();
  //return (int)((envelope.next() * aCarrier.phMod(modulation)) >> 8);
}


void loop(){
  audioHook();
}
// ADD
//
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
  display.print("A: ");
  display.print(noteA);

  // transpose
  display.setCursor(trans_text_pos.x, trans_text_pos.y);
  display.print("B: ");
  display.print(mod_to_carrier_ratio);

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

// second core setup
// second core dedicated to sample processing
void setup1() {
  delay (1000); // wait for main core to start up perhipherals
}

// second core calculates samples and sends to DAC
void loop1() {
  uint32_t now = millis();
  bool anybuttonpressed;


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

  anybuttonpressed = false;
  
  for (int i = 0; i <= 8; ++i) { // scan all the buttons
    if (button[i]) {
      pressedB = i;
      anybuttonpressed = true;

      // a track button is pressed
      current_track = i; // keypress selects track we are working on
      //  if ((!potlock[1]) || (!potlock[2])) seq[i].trigger=euclid(16,map(potvalue[1],POT_MIN,POT_MAX,0,MAX_SEQ_STEPS),map(potvalue[2],POT_MIN,POT_MAX,0,MAX_SEQ_STEPS-1));
      // look up drum trigger pattern encoder play modes
      if ( i == 2 && encoder_delta != 0 ){

          attackIn += encoder_delta * 5;
          //attackIn = constrain(attackIn, 0, 380);
       }
       if ( i == 3 && encoder_delta != 0 ){
          durationIn += encoder_delta * 5;
          //durationIn= constrain(durationIn, 16, 380);
          pressedB = 1;
       }
       if ( i == 4 && encoder_delta != 0){
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
        noteA = (map(mozziAnalogRead(AIN0), POT_MIN, POT_MAX, 200, 10000));
        //voice[current_track].sampleincrement = (uint16_t)(map(potvalue[0], POT_MIN, POT_MAX, 2048, 8192)); // change sample pitch if pot has moved enough
      }

      // change volume on pot 1
      if (display_mode == 0) {
        mod_to_carrier_ratio = (map(mozziAnalogRead(AIN1), POT_MIN, POT_MAX, 1, 20));
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

    }
  }

  // now, after buttons check if only encoder moved and no buttons
  if (! anybuttonpressed && encoder_delta) {
    bpm = bpm + encoder_delta; 
    display_value(bpm - 50);
    
    
  }

  
  /// only set new pos last after buttons have had a chance to use the delta
  encoder_pos_last = encoder_pos;
  encoder_delta = 0;  // we've used it

  // start tracking time encoder button held
  if ( ! digitalRead( SHIFTBUTTON ) ){ 
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
