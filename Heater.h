
class Heater {

  private:
    int pin;
  
  public:
    Heater(int pin);
    void setup();
    boolean processCommand(const char* cmd);
};
