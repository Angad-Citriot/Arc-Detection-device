#ifndef MQTT_H
#define MQTT_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

class MQTTClient {
public:
    MQTTClient(const char* ssid, const char* password, const char* mqttServer, int mqttPort, const char* topic);

    void begin();
    void publishData(float voltage, float current, float power, float powerFactor, float frequency, float arc);
    bool isConnected();
    
    void connectWiFi();  // ✅ Fix: Moved to public
    void connectMQTT();  // ✅ Fix: Moved to public

private:
    const char* _ssid;
    const char* _password;
    const char* _mqttServer;
    int _mqttPort;
    const char* _topic;
    
    WiFiClient _espClient;
    PubSubClient _client;
};

#endif
