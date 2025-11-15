// util int to char array
void int_to_char(int num, char *result) {
  int temp = num;
  int len = 0;
  while (temp > 0) {
    len++;
    temp /= 10;
  }
  for (int i = len - 1; i >= 0; i--) {
    result[i] = num % 10 + '0';
    num /= 10;
  }
  result[len] = '\0';
}

// Originaly from todbot's sequencer

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

//// {x,y} locations of play screen items
const int step_text_pos[] = { 0, 15, 16, 15, 32, 15, 48, 15, 64, 15, 80, 15, 96, 15, 112, 15 };
const pos_t bpm_text_pos    = {.x = 0,  .y = 15, .str = "bpm:%3d" };
const pos_t trans_text_pos  = {.x = 65, .y = 15, .str = "trs:%+2d" };
const pos_t seqno_text_pos  = {.x = 65, .y = 30, .str = "seq:%d" };
const pos_t seq_info_pos    = {.x = 0, .y = 30, .str = "" };

const pos_t play_text_pos   = {.x = 0, .y = 45, .str = "" };

const pos_t pat_text_pos    = {.x = 0, .y = 55,  .str = "" };
const pos_t pit_text_pos    = {.x = 65, .y = 55,  .str = "" };

const pos_t gate_bar_offset = { .x = 0, .y = -15, .str = "" };
const pos_t edit_text_offset = { .x = 3, .y = 22,  .str = "" };
const int gate_bar_width = 14;
const int gate_bar_height = 4;

void displayStart() {
  display.clearDisplay();

  display.cp437(true);

  // bpm
  display.setCursor(bpm_text_pos.x, bpm_text_pos.y);
  display.print("Detected BPM: ");
  display.print(bpm);
    // display load

  // seq info / meta
  display.setCursor(seq_info_pos.x, seq_info_pos.y);
  display.print("FILLS: ");
  display.print((String)currentConfig.fills[0]);

  // seqno
  display.setCursor(seqno_text_pos.x, seqno_text_pos.y);
  display.print("PITCH: ");
  display.print((String)currentConfig.sample[0]);  // user sees 1-8

  // seq info / meta
  display.setCursor(play_text_pos.x, play_text_pos.y);
  display.print("MODE: ");
  display.print(display_mode);
    
  display.setCursor(pat_text_pos.x, pat_text_pos.y);
  display.print("Loading: ");
  display.print(selected_preset);
  display.display();
}

void displayLoadSaveUpdate() {
  display.clearDisplay();

  display.cp437(true);

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
  display.print("OFSET: ");
  display.print(display_repeats);

  // seqno
  display.setCursor(seqno_text_pos.x, seqno_text_pos.y);
  display.print("PITCH: ");
  display.print(display_pitch);  // user sees 1-8

  // seq info / meta
  display.setCursor(play_text_pos.x, play_text_pos.y);
  display.print("MODE: ");
  display.print(display_mode);
  // preset
  display.setCursor(65, play_text_pos.y);
  display.print("Pre: ");
  display.print(selected_preset);

  // display load
  display.setCursor(pat_text_pos.x, pat_text_pos.y);
  display.print("Load: ");
  display.print(selected_preset);

  // display save
  display.setCursor(pit_text_pos.x, pat_text_pos.y);
  display.print("Save: ");
  display.print(selected_preset);
  // in save mode, default underline
  if (loadSave == 0) {
    display.setCursor(65, 60);
    display.print("_____");
  } else {
    display.setCursor(0, 60);
    display.print("_____");
  }

  /*
    display.setCursor(pat_text_pos.x, pat_text_pos.y);
    display.print("PAT: ");
    display.print((String)display_pat);
  */
  // play/pause
  //display.setCursor(play_text_pos.x, play_text_pos.y);
  //display.print(seqr.playing ? " >" : "[]");

  display.display();
}


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
  display.print("OFSET: ");
  display.print(display_repeats);

  // seqno
  display.setCursor(seqno_text_pos.x, seqno_text_pos.y);
  display.print("PITCH: ");
  display.print(display_pitch);  // user sees 1-8

  // seq info / meta
  display.setCursor(play_text_pos.x, play_text_pos.y);
  display.print("MODE: ");
  display.print(display_mode);

  display.setCursor(pat_text_pos.x, pat_text_pos.y);
  display.print("PAT: ");
  display.print(display_pat);

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
