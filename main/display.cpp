#include "display.h"

TFTDisplay::TFTDisplay() 
    : tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST),
      voltage(200), current(0), power(0), frequency(49), powerfactor(0) {}

void TFTDisplay::begin() {
  
    tft.begin();
    tft.setRotation(1);  
    tft.fillScreen(ILI9341_WHITE); 

    pinMode(TFT_LED, OUTPUT);
    digitalWrite(TFT_LED, HIGH);

    drawImage(0, 60);
    delay(500);
    drawSecondPage(0, 0);
}

void TFTDisplay::dispArc() {

    // tft.begin();
    // tft.setRotation(1);  
    // tft.fillScreen(ILI9341_WHITE); 

    // pinMode(TFT_LED, OUTPUT);
    // digitalWrite(TFT_LED, HIGH);

    // drawImage(0, 60);
    drawThirdPage(0, 0);
    delay(5000);
    drawSecondPage(0, 0);
}


void TFTDisplay::updateDisplay(double voltage, double current, double power, double frequancy, double powerfactor){
    dispValue(voltage, 180, 15);
    dispValue(current, 180, 75);
    dispValue(power, 180, 130, 2);
    dispValue(frequancy, 190, 193);
    dispValue(powerfactor, 20, 193);
    // updateValues();
}

void TFTDisplay::dispValue(double value, int x, int y, int precision) {
    tft.setCursor(x, y);
    tft.setTextSize(2);
    tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
    if (precision)
        tft.print(value, precision);
    else
        tft.print(value);
}



void TFTDisplay::drawImage(int x, int y) {
    tft.startWrite();
    for (int j = 0; j < IMG_HEIGHT; j++) {
        tft.setAddrWindow(x, y + j, IMG_WIDTH, 1);
        for (int i = 0; i < IMG_WIDTH; i++) {
            uint16_t color = pgm_read_word(&myImage[j * IMG_WIDTH + i]);
            tft.writeColor(color, 1);
        }
    }
    tft.endWrite();
}

void TFTDisplay::drawSecondPage(int x, int y) {
    tft.startWrite();
    for (int j = 0; j < MAIN_IMG_HEIGHT; j++) {
        tft.setAddrWindow(x, y + j, MAIN_IMG_WIDTH, 1);
        for (int i = 0; i < MAIN_IMG_WIDTH; i++) {
            uint16_t color = pgm_read_word(&mainDisp[j * MAIN_IMG_WIDTH + i]);
            tft.writeColor(color, 1);
        }
    }
    tft.endWrite();
}


void TFTDisplay::drawThirdPage(int x, int y) {
    tft.startWrite();
    for (int j = 0; j < MAIN_IMG_HEIGHT; j++) {
        tft.setAddrWindow(x, y + j, MAIN_IMG_WIDTH, 1);
        for (int i = 0; i < MAIN_IMG_WIDTH; i++) {
            uint16_t color = pgm_read_word(&page3[j * MAIN_IMG_WIDTH + i]);
            tft.writeColor(color, 1);
        }
    }
    tft.endWrite();
}

 