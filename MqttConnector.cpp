#include <MqttConnector.h>

MqttConnector::MqttConnector(
      char* wifi_ssid, 
      char* wifi_password, 
      char* mqtt_server,
      char* mqtt_port,
      char* mqtt_user,
      char* mqtt_pass,
      char* mqtt_client_name
    )
  : wifi_ssid(wifi_ssid), 
    wifi_password(wifi_password), 
    mqtt_server(mqtt_server), 
    mqtt_port(mqtt_port), 
    mqtt_user(mqtt_user), 
    mqtt_pass(mqtt_pass),
    mqtt_client_name(mqtt_client_name)
{
  wifi_client = &WiFiClient();
  mqtt_client = &PubSubClient(mqtt_server, mqtt_port, wifi_client);
}

boolean MqttConnector::connect(MQTT_CALLBACK_SIGNATURE, std::vector *topics) {
  return connectWiFi() && connectMqtt(callback, topics);
}

boolean MqttConnector::connectWiFi() {
  if (WiFi.waitForConnectResult() == WL_CONNECTED) return true;
  
  WiFi.begin(wifi_ssid, wifi_password);
  return WiFi.waitForConnectResult() == WL_CONNECTED;
}

boolean MqttConnector::connectMqtt(MQTT_CALLBACK_SIGNATURE, std::vector *topics) {
  if (mqtt_client.connected()) return true;
  if (!mqtt_client.connect(mqtt_client_name, mqtt_user, mqtt_pass)) return false;
    
  client.setCallback(callback);

  for (std::vector<T>::iterator it = topics->begin(); it != topics->end(); ++it) {
      client.subscribe(*it);
  }  
  return true;
}

char* MqttConnector::getConnectionError() {
  if (WiFi.status() != WL_CONNECTED) return "ERR >> WiFi connection failed.";
  else if (!mqtt_client.connected()) return "ERR >> MQTT-server connection failed.";
  else return null;
}

boolean MqttConnector::publish(const char* topic, const char* payload) {
  return mqtt_client.publish(topic, payload);
}

void MqttConnector::loop() {
  mqtt_client.loop();
}

