#pragma once
#define NUMBER_OF_STEPS 15

class euclid {

public:

  void generateSequence(uint8_t fills, uint8_t steps );
  void generateRandomSequence(uint8_t fills, uint8_t steps);
  void rotate(uint8_t _steps);
  bool getStep(uint8_t _step);
  uint8_t getStepNumber(){return stepCounter;};
  uint8_t getNumberOfFills(){return numberOfFills;};
  bool getCurrentStep();

  void doStep();
  void setRepeats(uint8_t repeats);
  uint8_t getRepeats();
  void setTextSequence();
  char getTextSequence();
  void resetSequence();
  char textSequence[NUMBER_OF_STEPS+1];
  
private:

  bool euclidianPattern[NUMBER_OF_STEPS+1];

  uint8_t stepCounter;
  uint8_t repeatCounter;
  uint8_t numberOfSteps;
  uint8_t numberOfFills;
  uint8_t repeats = 0;

};
