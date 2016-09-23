#include <DHT.h>
#include "MqttConnector.h"

struct SensorData {
  temperature: null,
  humidity: null,
  dewPoint: null
};

class TempSensor {

  private:
    DHT* sensor;

    // вычисляет точку росы
    float calcDewPoint(float t, float h);
  
  public:
    TempSensor(int pin);
    void setup();
    public SensorData read();
    boolean publish(const MqttConnector* mqtt, const char* topic);
};
