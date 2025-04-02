#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include "page1.h"
#include "page2.h"
#include "page3.h"

// Pin definitions
#define TFT_DC 2
#define TFT_RST 4
#define TFT_MOSI 11
#define TFT_CLK  12
#define TFT_CS  13
#define TFT_LED 21

class TFTDisplay {
private:
    Adafruit_ILI9341 tft;
    double voltage;
    double current;
    double power;
    double frequency;
    double powerfactor;
    // uint16_t voltage, current,  frequency, powerfactor;
    // float power;

public:
    TFTDisplay();
    void begin();
    void updateDisplay(double voltage, double current, double power, double frequancy, double powerfactor);
    void dispArc();
    
private:
    void dispValue(double value, int x, int y, int precision = 0);
    void updateValues();
    void drawImage(int x, int y);
    void drawSecondPage(int x, int y);
    void drawThirdPage(int x, int y);
};

#endif // DISPLAY_H
