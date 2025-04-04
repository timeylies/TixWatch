

/* Pin Definitions for TTGO T-Watch 2020 V3 */
#define TFT_BACKLIGHT 15 //you have to turn on LDO2 to turn the back light on
#define SENSOR_SDA 21 //for BMA423, PCF8563, and AXP202
#define SENSOR_SCL 22
#define FT6336_SDA 23
#define FT6336_SCL 32
#define FT6336_RST 14
#define VIBRATOR 4
#define BMA423_INTERRUPT 39
#define FT6336_INTERUPT 38
#define PCF8563_INTERUPT 37
#define AXP202_INTERUPT 35
#define I2C_BCK 26 //for the speaker, LDO4 needs to be turned on to use it
#define I2C_WS 25
#define I2C_DOUT 33
#define PDM_DATA 2 //for the mic
#define PDM_CLK 0
#define IR_BLASTER 13

/* Libraries */
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Wire.h>
#include <axp20x.h>
#include "focaltech.h"
#include <lvgl.h>
