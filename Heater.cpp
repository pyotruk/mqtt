#include <Heater.h>

Heater::Heater(int pin) : pin(pin) {}

void Heater::setup() {
  pinMode(pin, OUTPUT);
}

boolean Heater::processCommand(const char* cmd) {
  if (49 == *cmd) {
    digitalWrite(pin, LOW);
    return true;
      
  } else if (48 == *cmd) {
    digitalWrite(pin, HIGH);
    return true;
      
  } else {
    return false;
  }
}

