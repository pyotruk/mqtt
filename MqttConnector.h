#include <std>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

class MqttConnector {

  private:
    char* wifi_ssid;
    char* wifi_password;
    char* mqtt_server;
    char* mqtt_port;
    char* mqtt_user;
    char* mqtt_pass;
    char* mqtt_client_name;

    WiFiClient* wifi_client;
    PubSubClient* mqtt_client;

    boolean connectWiFi();
    boolean connectMqtt();
  
  public:
    MqttConnector(
      char* wifi_ssid, 
      char* wifi_password, 
      char* mqtt_server,
      char* mqtt_port,
      char* mqtt_user,
      char* mqtt_pass,
      char* mqtt_client_name
    );
    boolean connect(MQTT_CALLBACK_SIGNATURE, std::vector *topics);
    char* getConnectionError();
    boolean publish(const char* topic, const char* payload);
    void loop();
};
