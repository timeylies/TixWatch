#include "config.h"
#include "hardware.h"

void setup() {
  Serial.begin(115200);
  //run the hardware setup first here
  hardware_init();
  tft->fillScreen(TFT_BLACK);
  tft->drawRect(0, 0, tft->width(), tft->height(), TFT_GREEN);

  // Set "cursor" at top left corner of display (0,0) and select font 4
  tft->setCursor(0, 4, 4);

  // Set the font colour to be white with a black background
  tft->setTextColor(TFT_WHITE);

  // We can now plot text on screen using the "print" class
  tft->println(" Initialised default\n");
  tft->println(" White text");
  
  tft->setTextColor(TFT_RED);
  tft->println(" Red text");
  
  tft->setTextColor(TFT_GREEN);
  tft->println(" Green text");
  
  tft->setTextColor(TFT_BLUE);
  tft->println(" Blue text");
}

void loop() {
 
}

