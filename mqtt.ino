// Принимаем данные от MQTT-сервера
// и управляем (вкл/выкл) устройством на порте DEVICE

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

const char *ssid = "GT-9195"; // Имя вайфай точки доступа
const char *pass = "asdfghjk"; // Пароль от точки доступа

const char *mqtt_server = "m21.cloudmqtt.com"; // Имя сервера MQTT
const int mqtt_port = 10331; // Порт для подключения к серверу MQTT
const char *mqtt_user = "heyhey"; // Логи от сервер
const char *mqtt_pass = "123123"; // Пароль от сервера

const int HEATER_PIN = LED_BUILTIN; // порт реле на обогреватель
const int SOCKET1_PIN = 3;
const int TEMP_IN_PIN = 4;
const int TEMP_OUT_PIN = 5;

const int SERIAL_SPEED = 9600; // скорость сериал порта
const int SENSOR_PERIOD = 60; // секунд
const int CLIENT_NAME = "NEW-LIFE";


// Вычисляет точку росы
float calcDewPoint(float t, float h) {
  return t - ((1 - h/100) / 0.05);
}

// Обработка команды на обогреватель
void process_heater(uint8_t* payload, unsigned int length) {
  if (49 == *payload) {
    digitalWrite(HEATER_PIN, LOW);
      
  } else if (48 == *payload) {
    digitalWrite(HEATER_PIN, HIGH);
      
  } else {
    Serial.print("WARN >> heater >> Unexpected value.");
  }
}

struct SensorData {
  temperature: null,
  humidity: null,
  dewPoint: null
};

SensorData readSensor(DHT *sensor) {
  SensorData data = new SensorData();

  data.temperature = sensor.readTemperature();
  data.humidity = sensor.readHumidity();
  data.dewPoint = calcDewPoint(data.temperature, data.humidity);

  return data;
}

void publishSensors() {

    SensorData temp_in_data = readSensor(&temp_in_dht);
    SensorData temp_out_data = readSensor(&temp_out_dht);

//TODO

    String msg = "T=";
    msg += temperature;
    msg += "___H=";
    msg += humidity;
    msg += "___Tp=";
    msg += dewPoint;

    char data[42];
    msg.toCharArray(data, 42);

    if (client.publish(mqtt_topic, data)) {
      Serial.println("OK >> published.");
      
    } else {
      Serial.println("ERR >> publish failed.");
    }    
}

void process(char* topic, uint8_t* payload, unsigned int length) {
  switch(topic) {
    case "heater": 
      process_heater(payload, length);
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

// Функция получения данных от сервера
void read_callback(char* topic, uint8_t* payload, unsigned int length) {
  Serial.print(topic); // выводим в сериал порт название топика
  Serial.print(" => ");
  Serial.print((char *)payload); // выводим в сериал порт значение полученных данных

  process(topic, payload, length);
}

void wifiConnect() {
    Serial.print("DEBUG >> Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("ERR >> WiFi connect failed.");
      return false;
    }
    
    Serial.println("OK >> WiFi connected");
    return true;
}

void mqttConnect() {
    Serial.println("DEBUG >> Connecting to MQTT server...");
     
    if (!client.connect(CLIENT_NAME, mqtt_user, mqtt_pass)) {
      Serial.println("ERR >> Could not connect to MQTT server");
      return false;
    }
      
    Serial.println("OK >> Connected to MQTT server");
    
    client.setCallback(read_callback);
    
    client.subscribe("heater");
    client.subscribe("socket1");
    client.subscribe("temp_in");
    client.subscribe("temp_out");
    client.subscribe("options");

    return true;
}

WiFiClient wclient; 
PubSubClient client(mqtt_server, mqtt_port, wclient);
DHT temp_in_dht(TEMP_IN_PIN, DHT22, 15);
DHT temp_out_dht(TEMP_OUT_PIN, DHT22, 15);

int count = 0;


void setup() {
  Serial.begin(SERIAL_SPEED);
  delay(10);
  Serial.println("WAIT >> Setup begin...");
  Serial.println();
  pinMode(HEATER_PIN, OUTPUT);
  temp_in_dht.begin();
  temp_out_dht.begin();
  Serial.println("OK >> Setup completed!");
}

void loop() {

  if (WiFi.status() != WL_CONNECTED && !wifiConnect()) {
    Serial.println("DEBUG >> WiFi is not connected, skipping...");
    return;
  }
  
  if (!client.connected() && !mqttConnect()) {
    Serial.println("DEBUG >> MQTT is not connected, skipping...");
    return;
  }

  // read sensors loop
  if ((++count % (10 * SENSOR_PERIOD)) == 0) {
    publishSensors();
  }

  client.loop();
  delay(100);
}

