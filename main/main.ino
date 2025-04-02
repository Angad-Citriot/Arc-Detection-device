#include "display.h"
#include <energyic_UART.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include "mqtt.h"

#define SDA_PIN 8  // Define SDA pin
#define SCL_PIN 9  // Define SCL pin

#define PIN_SerialATM_RX 39   // RX pin for ESP32-S3
#define PIN_SerialATM_TX 38   // TX pin for ESP32-S3
HardwareSerial ATMSerial(1);

ATM90E26_UART eic(&ATMSerial);
float arc = 0;
TFTDisplay display;

Adafruit_ADS1115 ads; 

// const char* ssid = "ACTFIBERNET";
// const char* password = "act12345";
const char* ssid = "Citriot";
const char* password = "citriot@123";
const char* mqttServer = "3.140.215.14";
const int mqttPort = 1883;
const char* topic = "ARCD1";

MQTTClient mqtt(ssid, password, mqttServer, mqttPort, topic);

// FreeRTOS Task Handles
TaskHandle_t wifiTaskHandle;
TaskHandle_t sensorTaskHandle;

void wifiTask(void* parameter) {
    while (1) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Attempting to connect to WiFi...");
            mqtt.connectWiFi();
        }

        if (!mqtt.isConnected()) {
            Serial.println("Attempting to connect to MQTT...");
            mqtt.connectMQTT();
        }

        vTaskDelay(10000 / portTICK_PERIOD_MS); // Retry every 10 seconds
    }
}

void sensorTask(void* parameter) {
  while (1) {
    // Read sensor values
    unsigned short s_status = eic.GetSysStatus();
    Serial.print("Sys Status: ");
    Serial.println(s_status, HEX);

    unsigned short meterStatus = eic.GetMeterStatus();
    Serial.print("Meter Status: ");
    Serial.println(meterStatus, HEX);

    float voltage = eic.GetLineVoltage() + 60.0;
    float current = eic.GetLineCurrent() / 4;
    float power = eic.GetActivePower() / 1000;
    float frequency = eic.GetFrequency();
    float powerFactor = eic.GetPowerFactor();

    // Ensure no negative values
    if (voltage <= 5) {
        voltage = 0;
        current = 0;
        power = 0;
        powerFactor = 0;
        frequency = 0;
    }
    else if (voltage > 500) {
        voltage = 0;
        current = 0;
        power = 0;
        powerFactor = 0;
        frequency = 0;
    }

    if (current < 0) current = 0;
    if (power <= 0) power = 0;
    if (powerFactor <= 0) powerFactor = 0;

    // Update display
    display.updateDisplay(voltage, current, power, frequency, powerFactor);

    int16_t adc_value = ads.readADC_SingleEnded(1); // Read from A1
    Serial.println(adc_value);

    if (adc_value > 15000 && adc_value < 20500) {
      Serial.println("ARC detected! Enjoy the moment.");
      arc = 1;
      display.dispArc();
    }

    // Publish data if MQTT is connected
    if (mqtt.isConnected()) {
        mqtt.publishData(voltage, current, power, powerFactor, frequency,arc);
        Serial.println("Data Published");
    } else {
        Serial.println("MQTT not connected, skipping publish...");
    }

    // Delay for 1 second
    vTaskDelay(10 / portTICK_PERIOD_MS); // Publish every 1 second
    arc = 0;
  }
}

void setup() { 
    display.begin();
    Serial.begin(115200);
    Serial.println("\nATM90E26 UART Test Started");

    ATMSerial.begin(9600, SERIAL_8N1, PIN_SerialATM_RX, PIN_SerialATM_TX);
    eic.InitEnergyIC();

    Serial.println("Starting ADS1X15...");
    Wire.begin(SDA_PIN, SCL_PIN); // Initialize I2C with custom pins
    
    if (!ads.begin()) {
        Serial.println("Failed to initialize ADS1X15!");
        while (1);
    }
    Serial.println("ADS1X15 Initialized.");
    mqtt.begin();

    // Create WiFi task on Core 1
    xTaskCreatePinnedToCore(
        wifiTask, "WiFiTask", 4096, NULL, 1, &wifiTaskHandle, 1); // Core 1

    // Create Sensor task on Core 0
    xTaskCreatePinnedToCore(
        sensorTask, "SensorTask", 4096 * 2, NULL, 1, &sensorTaskHandle, 0); // Core 0

    delay(1000);
}

void loop() {
  // Nothing here as tasks are handled in FreeRTOS
}
