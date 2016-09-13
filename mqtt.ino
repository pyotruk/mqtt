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
const char *mqtt_topic = "test"; // Название топика

const int DEVICE = LED_BUILTIN; // порт утсройства, которым мы управляем (светодиод или реле)

// Функция получения данных от сервера
void callback(char* topic, uint8_t* payload, unsigned int length) {
  Serial.print(topic); // выводим в сериал порт название топика
  Serial.print(" => ");
  Serial.print((char *)payload); // выводим в сериал порт значение полученных данных

  if (49 == *payload) {
    digitalWrite(DEVICE, LOW);
      
  } else if (48 == *payload) {
    digitalWrite(DEVICE, HIGH);
      
  } else {
    Serial.print("WARN >> Unexpected value.");
  }
}

WiFiClient wclient; 
PubSubClient client(mqtt_server, mqtt_port, wclient);
DHT dht(2, DHT22, 15);

int cnt = 0;

void setup() {
  Serial.begin(9600);
  delay(10);
  Serial.println("WAIT >> Setup begin...");
  Serial.println();
  pinMode(DEVICE, OUTPUT);
  dht.begin();
  Serial.println("OK >> Setup completed!");
}

void loop() {
  
  // подключаемся к wi-fi
  if (WiFi.status() != WL_CONNECTED) {
    
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) return;
    
    Serial.println("WiFi connected");
  }

  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("DEBUG >> WiFi is not connected, skipping...");
    return;
  }
  
  if (!client.connected()) { // подключаемся к MQTT серверу  
      
    Serial.println("Connecting to MQTT server");
     
    if (client.connect("NEW-LIFE", mqtt_user, mqtt_pass)) {
      
      Serial.println("Connected to MQTT server");
      
      client.setCallback(callback);
      client.subscribe(mqtt_topic); // подписываемся на топик с данными для светодиода
        
    } else {
      Serial.println("Could not connect to MQTT server"); 
    }
  }

  if (client.connected() && ((++cnt % 1000) == 0)) {
    
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    String msg = "T=";
    msg += temperature;
    msg += ", H=";
    msg += humidity;

    if (client.publish(mqtt_topic, (char*)&msg)) {
      Serial.println("OK >> published.");
      
    } else {
      Serial.println("ERR >> publish failed.");
    }    
  }

  if (client.connected()) {
    client.loop();
  }

}

