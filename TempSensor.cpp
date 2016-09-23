#include <TempSensor.h>

TempSensor::TempSensor(int pin)
  : sensor(&DHT(pin, DHT22, 15))
{}

void TempSensor::setup() {
  sensor->begin();
}

SensorData TempSensor::read() {
  SensorData data = new SensorData();

  data.temperature = sensor->readTemperature();
  data.humidity = sensor->readHumidity();
  data.dewPoint = calcDewPoint(data.temperature, data.humidity);

  return data;
}

boolean TempSensor::publish(const MqttConnector* mqtt, const char* topic) {
  SensorData data = sensor->read();

  String msg = "T=";
  msg += data.temperature;
  msg += "___H=";
  msg += data.humidity;
  msg += "___Tp=";
  msg += data.dewPoint;

  unsigned long int length = sizeof(msg);
  char payload[length];
  msg.toCharArray(payload, length);

  return mqtt.publish(topic, payload);
}

float TempSensor::calcDewPoint(float t, float h) {
  return t - ((1 - h/100) / 0.05);
}

