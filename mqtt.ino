#include "TempSensor.h"
#include "Heater.h"
#include "MqttConnector.h"

const char *WIFI_SSID = "GT-9195"; // Имя вайфай точки доступа
const char *WIFI_PASSWORD = "asdfghjk"; // Пароль от точки доступа

const char *MQTT_SERVER = "m21.cloudmqtt.com"; // Имя сервера MQTT
const int MQTT_PORT = 10331; // Порт для подключения к серверу MQTT
const char *MQTT_USER = "heyhey"; // Логи от сервер
const char *MQTT_PASSWORD = "123123"; // Пароль от сервера
const int MQTT_CLIENT_NAME = "NEW-LIFE";

const int HEATER_PIN = LED_BUILTIN; // порт реле на обогреватель
const int SOCKET1_PIN = 3;
const int TEMP_IN_PIN = 4;
const int TEMP_OUT_PIN = 5;

const int SERIAL_SPEED = 9600; // скорость сериал порта
const int SENSOR_PERIOD = 60; // период отправки показаний датчиков в секундах


// задаём топики
std::vector<string> topics;
topics.push_back("heater");
topics.push_back("socket1");
topics.push_back("temp_in");
topics.push_back("temp_out");
topics.push_back("options");

Heater heater(HEATER_PIN);
TempSensor temp_in(TEMP_IN_PIN);
TempSensor temp_out(TEMP_OUT_PIN);
MqttConnector mqtt(WIFI_SSID, WIFI_PASSWORD, MQTT_SERVER, MQTT_PORT, MQTT_USER, MQTT_PASSWORD, MQTT_CLIENT_NAME);

int count = 0;

// Функция получения данных от сервера
void read_callback(char* topic, uint8_t* payload, unsigned int length) {
  Serial.print(topic); // выводим в сериал порт название топика
  Serial.print(" => ");
  Serial.print((char *)payload); // выводим в сериал порт значение полученных данных

  switch(topic) {
    case "heater": 
      int ok = heater.processCommand(payload);
      if (!ok) Serial.println("ERR >> heater command failed.");
      break;
    case "socket1": 
      break;
    case "temp_in": 
      break;
    case "temp_out": 
      break;
    case "options": 
      break;
  }
}

void setup() {
  Serial.begin(SERIAL_SPEED);
  delay(10);
  Serial.println("WAIT >> Setup in progress...");
  
  heater->setup();
  tempIn->setup();
  tempOut->setup();
  
  Serial.println("OK >> Setup completed!");
}

void loop() {

  if (!mqtt->connect(read_callback, topics)) {
    Serial.println(mqtt->getConnectionError());
    return;
  }

  Serial.println("OK >> MQTT-server and WiFi connected.");
  
  // read sensors loop
  if ((++count % (10 * SENSOR_PERIOD)) == 0) {
    boolean ok = temp_in->publish(mqtt, "temp_in");
    ok = ok && temp_out->publish(mqtt, "temp_out");
    
    if(!ok) Serial.println("ERR >> Failed to publish sensors data.");
  }

  mqtt.loop();
  delay(100);
}

