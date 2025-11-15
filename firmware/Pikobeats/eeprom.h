/* EEPROM  mostly from modulove A-RYTH-MATIK
  { fills } { offsets } volumes, pitch selected sample, bpm, ExtClock
  // Define only the first 10 presets for LGT8FX_BOARD
*/
// we save preset & slot before this
#define EEPROM_START_ADDRESS 50
#define NUM_PRESETS 8
#define CONFIG_SIZE (sizeof(ConfigSlot))
#define LAST_USED_SLOT_ADDRESS (EEPROM_START_ADDRESS + NUM_PRESETS * CONFIG_SIZE)


// we have 8 preset progmem and save altered/new settings to eeprom slots

struct ConfigSlot {
  byte fills[8];         // Number of hits per pattern in each channel
  byte offset[8];       // Step offset for each channel
  byte randy[8];
  byte volume[8];        // Step limit (length) of the pattern for each channel
  byte pitch[8];  // channels pitch saved as 8 bits << 5
  byte sample[8];  // 0 - 32
  int tempo;                       // Tempo for the preset
  bool internalClock;              // Clock source state
  byte lastUsedSlot;               // Last used slot
  byte selectedPreset;             // last used preset / slot > 10 is an eeprom slot
};

const ConfigSlot defaultSlots[NUM_PRESETS] = PROGMEM {
  { { 4, 6, 5, 4, 3, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0}, { 0, 0, 0, 1, 0, 0, 0, 0},  {175, 175, 175, 175, 175, 175, 175, 175}, {128, 128, 128, 128, 128, 128, 128, 128}, {0, 1, 2, 3, 4, 5, 6, 7}, 120, 0 , 0, 0 },
  { { 4, 3, 5, 4, 3, 0, 0, 0 }, { 0, 2, 1, 0, 0, 0, 0, 0}, { 0, 0, 0, 1, 0, 0, 0, 0},  {175, 175, 175, 175, 175, 175, 175, 175}, {128, 128, 128, 128, 128, 128, 128, 128}, {0, 1, 2, 3, 4, 5, 6, 7}, 100, 0 , 0, 0 },
  { { 6, 2, 5, 4, 3, 0, 0, 0 }, { 0, 1, 0, 0, 0, 0, 0, 0}, { 0, 0, 1, 0, 0, 0, 0, 0},  {175, 175, 175, 175, 175, 175, 175, 175}, {128, 128, 128, 128, 128, 128, 128, 128}, {0, 1, 2, 3, 4, 5, 6, 7},  90, 0 , 0, 0 },
  { { 4, 4, 5, 6, 3, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0}, { 0, 0, 0, 1, 0, 0, 0, 0},  {175, 175, 175, 175, 175, 175, 175, 175}, {128, 128, 128, 128, 128, 128, 128, 128}, {0, 1, 2, 3, 4, 5, 6, 7},  80, 0 , 0, 0 },
  { { 6, 5, 4, 4, 3, 0, 0, 0 }, { 0, 1, 0, 0, 0, 0, 0, 0}, { 0, 0, 0, 1, 0, 0, 0, 0},  {175, 175, 175, 175, 175, 175, 175, 175}, {128, 128, 128, 128, 128, 128, 128, 128}, {0, 1, 2, 3, 4, 5, 6, 7}, 130, 0 , 0, 0 },
  { { 2, 4, 6, 2, 4, 0, 0, 0 }, { 1, 0, 0, 0, 0, 0, 0, 0}, { 0, 1, 0, 0, 0, 0, 0, 0},  {175, 175, 175, 175, 175, 175, 175, 175}, {128, 128, 128, 128, 128, 128, 128, 128}, {0, 1, 2, 3, 4, 5, 6, 7}, 150, 0 , 0, 0 },
  { { 2, 6, 2, 4, 3, 2, 2, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0}, { 0, 0, 0, 1, 0, 0, 0, 0},  {175, 175, 175, 175, 175, 175, 175, 175}, {128, 128, 128, 128, 128, 128, 128, 128}, {0, 1, 2, 3, 4, 5, 6, 7},  90, 0 , 0, 0 },
  { { 4, 4, 3, 4, 3, 1, 2, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0}, { 0, 0, 0, 1, 0, 0, 0, 0},  {175, 175, 175, 175, 175, 175, 175, 175}, {128, 128, 128, 128, 128, 128, 128, 128}, {0, 1, 2, 3, 4, 5, 6, 7}, 100, 0 , 0, 0 },
};

ConfigSlot memorySlots[NUM_PRESETS], currentConfig;

uint8_t internalClock = 0;
uint8_t lastUsedSlot = 0;
bool offset_buf[8][16];  //offset buffer , Stores the offset result

// selected_preset = (selected_preset + increment + sizeof(defaultSlots) / sizeof(ConfigSlot)) % (sizeof(defaultSlots) / sizeof(ConfigSlot));

// update all the seq objects with fills and offsets
void updateRythm() {
  for (uint8_t i = 0; i < 8; ++i) {
    uint8_t hits = currentConfig.fills[i];
    uint8_t offset = currentConfig.offset[i];
    uint8_t sample = currentConfig.sample[i];
    int pitch = ( currentConfig.pitch[i] << 5);
    int level = ( currentConfig.volume[i] << 2) ;
    int randy = currentConfig.randy[i];

    seq[i].fills = hits;
    seq[i].repeats = offset;
    if (randy == 0 && hits > 0) {
      seq[i].trigger->generateSequence(hits, 16); // yeah, why is this fixed to 16?
    } else if (randy == 1 && hits > 0) {
      if (debug) Serial.println("random pattern");
      seq[i].trigger->generateRandomSequence(hits, 16);
    }

    if (offset != 0 && hits > 0) seq[i].trigger->rotate(offset);

    // now set sample properties
    voice[i].sampleincrement = pitch; // stored as 8 bit
    voice[i].level = level; // 0 - 1023 also 8 bits
    voice[i].sample = sample;
    if (debug) {
      Serial.println(hits);
      Serial.println(offset);
      Serial.println(level);
      Serial.println(pitch);
      Serial.println(sample);
    }

  }
  if (debug) Serial.println("updated patterns");



}

// Loading ConfigSlot from PROGMEM
void loadDefaultConfig(ConfigSlot *config, int index) {
  if (index >= NUM_PRESETS) index = 0;
  memcpy_P(config, &defaultSlots[index], sizeof(ConfigSlot));

}

void loadFromPreset(int preset) {
  if (preset >= sizeof(defaultSlots) / sizeof(ConfigSlot)) preset = 0;
  loadDefaultConfig(&currentConfig, preset);
  bpm = currentConfig.tempo;
  internalClock = currentConfig.internalClock;
  if (debug) Serial.println(bpm);
  selected_preset = preset;
}

// Loading ConfigSlot from memorySlots
void loadMemoryConfig(ConfigSlot *config, int index) {
  if (index >= NUM_PRESETS) index = 0;
  memcpy(config, &memorySlots[index], sizeof(ConfigSlot));
}

void loadFromMemorySlot(int preset) {
  if (preset >= sizeof(memorySlots) / sizeof(ConfigSlot)) preset = 0;
  //loadMemoryConfig(&currentConfig, preset);

  currentConfig = memorySlots[preset];

  bpm = currentConfig.tempo;
  internalClock = currentConfig.internalClock;
  selected_preset = preset;

  //period = 60000 / bpm / 4;
  //setup_complete = true;

  updateRythm();

}

// track of current preset and slot outside of the general config
// permit loading correct preset/slot at boot with less overhead
void saveCurrentPreset(int preset) {
  uint8_t baseAddress = 10;
  EEPROM.write(baseAddress, preset);
  if (EEPROM.commit()) {
    if (debug) Serial.println("EEPROM wrote preset");
  } else {
    if (debug) Serial.println("ERROR! EEPROM commit failed");
  }
}

void loadLastPreset() {
  uint8_t baseAddress = 10;
  selected_preset = EEPROM.read(baseAddress);
  if (debug) Serial.print("Last preset: ");
  if (debug) Serial.println(selected_preset);
}


void loadInit() {
  uint8_t baseAddress = 20;
  device_initialized = EEPROM.read(baseAddress);
  if (debug) Serial.print("Last preset: ");
  if (debug) Serial.println(selected_preset);
}

void writeInit() {
  uint8_t baseAddress = 20;
  EEPROM.write(baseAddress, 7);
  if (EEPROM.commit()) {
    if (debug) Serial.println("EEPROM wrote preset");
  } else {
    if (debug) Serial.println("ERROR! EEPROM commit failed");
  }
}

void saveToEEPROM(int slot) {
  int baseAddress = EEPROM_START_ADDRESS + (slot * CONFIG_SIZE);
  if (baseAddress + CONFIG_SIZE <= EEPROM.length()) {
    currentConfig.tempo = bpm;
    currentConfig.internalClock = internalClock;
    currentConfig.lastUsedSlot = slot;
    currentConfig.selectedPreset = selected_preset;
    EEPROM.put(baseAddress, currentConfig);
  } else {
    // Handle error
    if (debug ) Serial.println("EEPROM Save Error");
    return;
  }
  if (EEPROM.commit()) {
    if (debug) Serial.println("EEPROM successfully committed");
    saveCurrentPreset(slot);  // update the currently set preset

  } else {
    if (debug) Serial.println("ERROR! EEPROM commit failed");
  }

}

/*
   This function takes the defaul prormem configs and copies them to eeprom
   Thereafter, we can read all eeprom settings at start to reduce noise on loading
*/

void initializeEEPROM() {
  //ConfigSlot conf;
  for (uint8_t slot = 0; slot < 8; slot++) {
    currentConfig = defaultSlots[slot];
    saveToEEPROM(slot);
    delay(50);

  }
}

/*
   On device start we read all eeprom slots to be abloe to access them without recourse to eeprom
*/

void loadMemorySlots() {

  for (uint8_t slot = 0; slot < 8; slot++) {
    int baseAddress = EEPROM_START_ADDRESS + (slot * CONFIG_SIZE);

    if (baseAddress + CONFIG_SIZE <= EEPROM.length()) {
      EEPROM.get(baseAddress, currentConfig);
      // don't populate garbage
      if (currentConfig.tempo > 0) {
        memorySlots[slot] = currentConfig;
      } else {
        memorySlots[slot] = defaultSlots[slot];
      }

    } else {
      // Handle the error
      if (debug ) Serial.println("EEPROM Save Error");
      return;
    }
  }
}
void loadMemorySlotDefaults() {
  for (uint8_t i = 0; i < 8; i++) {
    //memorySlots[i] = defaultSlots[i];
    memcpy(&memorySlots[i], &defaultSlots[i], sizeof(ConfigSlot));
    if (debug) Serial.println(memorySlots[i].tempo);
  }
}

/*
   On save, update memorySlots

*/
void updateMemorySlot(int preset) {
  memorySlots[preset] = currentConfig;
}


void loadFromEEPROM(int slot) {
  int baseAddress = EEPROM_START_ADDRESS + (slot * CONFIG_SIZE);
  if (baseAddress + CONFIG_SIZE <= EEPROM.length()) {
    EEPROM.get(baseAddress, currentConfig);
    bpm = currentConfig.tempo;
    internalClock = currentConfig.internalClock;
    //lastUsedSlot = slot;
    //selected_preset = currentConfig.selectedPreset;
    if (debug) Serial.println(bpm);
    //period = 60000 / bpm / 4;  // Update period with loaded tempo
    //setup_complete = true;
    updateRythm();               // Refresh

  } else {
    // Handle the error
    if (debug ) Serial.println("EEPROM Save Error");
    return;
  }
}

void initializeCurrentConfig(bool loadDefaults = false) {
  if (loadDefaults) {
    // Load default configuration from PROGMEM
    memcpy(&currentConfig, &defaultSlots[0], sizeof(ConfigSlot));  // Load the first default slot as the initial configuration
  } else {
    // Load configuration from EEPROM
    int baseAddress = EEPROM_START_ADDRESS;  // Start address for the first slot
    EEPROM.get(baseAddress, currentConfig);
    bpm = currentConfig.tempo;                     // Load tempo
    internalClock = currentConfig.internalClock;     // Load clock state
    lastUsedSlot = currentConfig.lastUsedSlot;       // Load last used slot
    selected_preset = currentConfig.selectedPreset;  // Load last used preset
    if (selected_preset < NUM_PRESETS) {             // we use overflow to determine if we're in eeprom
      loadFromPreset(selected_preset);
    } else {
      loadFromEEPROM(lastUsedSlot);
    }
  }
}
