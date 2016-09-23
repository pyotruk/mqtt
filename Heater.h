#include <Arduino.h>

class Heater {

  private:
    int pin;
  
  public:
    Heater(int pin);
    void setup();
    bool processCommand(const char* cmd);
};
