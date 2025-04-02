#include "mqtt.h"

MQTTClient::MQTTClient(const char* ssid, const char* password, const char* mqttServer, int mqttPort, const char* topic)
    : _ssid(ssid), _password(password), _mqttServer(mqttServer), _mqttPort(mqttPort), _topic(topic), _client(_espClient) {}

void MQTTClient::begin() {
    _client.setServer(_mqttServer, _mqttPort);
}

void MQTTClient::publishData(float voltage, float current, float power, float powerFactor, float frequency, float arc) {
    if (!_client.connected()) {
        return;
    }

    StaticJsonDocument<256> doc;
    doc["voltage"] = voltage;
    doc["current"] = current;
    doc["power"] = power;
    doc["powerFactor"] = powerFactor;
    doc["frequency"] = frequency;
    doc["arc"] = arc;
    
    char buffer[256];
    serializeJson(doc, buffer);
    
    _client.publish(_topic, buffer);
}

void MQTTClient::connectWiFi() {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(_ssid, _password);

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 20) {
        delay(500);
        Serial.print(".");
        retries++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected!");
    } else {
        Serial.println("\nWiFi connection failed!");
    }
}

void MQTTClient::connectMQTT() {
    if (!_client.connected()) {
        Serial.print("Connecting to MQTT...");
        if (_client.connect("ESP32_Client")) {
            Serial.println("Connected!");
        } else {
            Serial.println("MQTT connection failed!");
        }
    }
}

bool MQTTClient::isConnected() {
    return _client.connected();
}
