#ifdef TWATCHV3
//to shut up the touch_cs pin warning
#define DISABLE_ALL_LIBRARY_WARNINGS
#define ST7789_DRIVER
#define TFT_WIDTH  240
#define TFT_HEIGHT 240
#define TFT_MISO            -1
#define TFT_MOSI            19
#define TFT_SCLK            18
#define TFT_CS              5
#define TFT_DC              27
#define SPI_FREQUENCY  80000000
#endif