#include "esp_sleep.h"

// Screen stuff

// to shut the errors up
#define TFT_WIDTH 240
#define TFT_HEIGHT 240

// TFT_eSPI *tft = nullptr; //not used since lvgl initializes tft_espi
#define DRAW_BUF_SIZE (TFT_WIDTH * TFT_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 2];
#define TFT_ROTATION LV_DISPLAY_ROTATION_180

int currentBrightness = 100;

void display_change_brightness(int brightness)
{
    currentBrightness = brightness;
    int mappedBrightness = map(brightness, 0, 100, 1, 255);
    analogWrite(TFT_BACKLIGHT, mappedBrightness);
    ESP_LOGI("Display", "Brightness changed to %i%%", brightness);
}

// Touch stuff

FocalTech_Class *touch;
TwoWire ti2c = TwoWire(1);

void init_touchpad()
{
    ti2c.begin(FT6336_SDA, FT6336_SCL, 100000);
    touch = new FocalTech_Class;
    if (!touch->begin(ti2c))
    {
        ESP_LOGW("Init Touchpad", "Uh oh, no touchpad found?");
    }
    else
    {
        ESP_LOGI("Init Touchpad", "Found touchpad!");
    }
}

// LVGL Stuff

uint32_t lastActivityMillis = 0;
bool screenDimmed = false; // true while showing the pre-sleep dim warning

void hardware_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    uint16_t x, y;
    bool touched = touch->getTouched();
    touch->getPoint(x, y);
    if (x > 240 || x < 0)
    {
        return;
    }
    if (y > 240 || y < 0)
    {
        return;
    }

    if (!touched)
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }
    else
    {
        lastActivityMillis = millis();
        if (screenDimmed)
        {
            screenDimmed = false;
            display_change_brightness(lv_slider_get_value(controlPanel_brightnessSlider));
        }
        data->state = LV_INDEV_STATE_PRESSED;

        // data->point.x = x;
        // data->point.y = y;
        // flipped so:
        data->point.x = map(x, 0, 240, 240, 0);
        data->point.y = map(y, 0, 240, 240, 0);
        // log_i("X: %i, Y: %i", x, y);
    }
}

static uint32_t lvgl_tick(void)
{
    return millis();
}

void lvgl_log_cb(lv_log_level_t level, const char *buf)
{
    switch (level)
    {
    case LV_LOG_LEVEL_TRACE:
        ESP_LOGV("LVGL", "%s", buf);
        break;
    case LV_LOG_LEVEL_INFO:
        ESP_LOGI("LVGL", "%s", buf);
        break;
    case LV_LOG_LEVEL_WARN:
        ESP_LOGW("LVGL", "%s", buf);
        break;
    case LV_LOG_LEVEL_ERROR:
        ESP_LOGE("LVGL", "%s", buf);
        break;
    case LV_LOG_LEVEL_USER:
        ESP_LOGI("LVGL", "%s", buf);
        break;
    }
}

// AXP stuff

AXP20X_Class *power;
bool axp_irq = false;

void init_power()
{
    power = new AXP20X_Class();
    Wire.begin(SENSOR_SDA, SENSOR_SCL);
    int ret = power->begin(Wire, AXP202_SLAVE_ADDRESS);
    if (ret == AXP_FAIL)
    {
        while (1)
        {
            ESP_LOGE("Init Power", "AXP Power begin failed!");
            delay(1000);
        }
    }
    power->limitingOff();

    // Audio power is LDO4, keep it off for now
    power->setPowerOutPut(AXP202_LDO4, false);
    power->setLDO4Voltage(AXP202_LDO4_3300MV);
    // No use
    power->setPowerOutPut(AXP202_LDO3, false);
    // turn on lcd backlight
    power->setPowerOutPut(AXP202_LDO2, AXP202_ON);

    // do some IRQ Stuff for checking button and usb and stuff
    pinMode(AXP202_INTERUPT, INPUT_PULLUP);
    power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ | AXP202_CHARGING_IRQ, true);
    power->clearIRQ();
    // for monitoring
    power->adc1Enable(
        AXP202_VBUS_VOL_ADC1 |
            AXP202_VBUS_CUR_ADC1 |
            AXP202_BATT_CUR_ADC1 |
            AXP202_BATT_VOL_ADC1,
        true);
}

// RTC Stuff
RTC_PCF8563 rtc;

void init_rtc()
{
    if (!rtc.begin())
    {
        ESP_LOGW("Init RTC", "Couldn't find RTC!");
        return;
    }
    else
    {
        ESP_LOGI("Init RTC", "Found RTC!");
    }
    if (rtc.lostPower())
    {
        ESP_LOGI("Init RTC", "RTC is NOT initialized, setting the time to compilation time...");
        // When time needs to be set on a new device, or after a power loss, the
        // following line sets the RTC to the date & time this sketch was compiled
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    rtc.start();
}

// Screen timeout

int screenTimeoutSeconds = 0; // 0 = never

// Light sleep

void enter_light_sleep()
{
    int savedBrightness = lv_slider_get_value(controlPanel_brightnessSlider);

    // disconnect WiFi before sleeping to save power; remember state so we can restore it
    bool wifiWasConnected = (WiFi.status() == WL_CONNECTED);
    if (wifiWasConnected)
        WiFi.disconnect(true);

    // dim the screen down to off, starting from wherever it actually is (may already be pre-dimmed)
    for (int b = currentBrightness; b >= 0; b -= 4)
    {
        display_change_brightness(b);
        delay(8);
    }
    analogWrite(TFT_BACKLIGHT, 0);                  // display_change_brightness floors at 1/255, force fully off
    power->setPowerOutPut(AXP202_LDO2, AXP202_OFF); // cut the backlight rail to save power

    // clear any pending button IRQ so we don't wake immediately
    power->readIRQ();
    power->clearIRQ();

    // wake when the side button is pressed again (AXP202 IRQ line is active low)
    esp_sleep_enable_ext0_wakeup((gpio_num_t)AXP202_INTERUPT, 0);
    esp_light_sleep_start();

    // ---- execution resumes here once woken ----
    power->setPowerOutPut(AXP202_LDO2, AXP202_ON);
    delay(20); // let the backlight rail stabilize

    for (int b = 0; b <= savedBrightness; b += 4)
    {
        display_change_brightness(b);
        delay(8);
    }
    display_change_brightness(savedBrightness); // land exactly on the original value (step may overshoot/undershoot)

    // reconnect WiFi if it was connected before sleep (uses saved credentials from last WiFi.begin())
    if (wifiWasConnected)
        WiFi.begin();
    WiFi.setTxPower(WIFI_POWER_19_5dBm);

    // clear the IRQ that woke us so the main loop doesn't treat it as another button press
    power->readIRQ();
    power->clearIRQ();
    lastActivityMillis = millis(); // reset timeout so we don't immediately sleep again
    screenDimmed = false;
}

// Make sure the library https://github.com/pschatzmann/arduino-audio-tools.git is in the ini

// Audio stuff
I2SStream i2s;

MemoryStream charging_wav(charging, sizeof(charging));
EncodedAudioStream out(&i2s, new WAVDecoder());

void init_audio()
{
    // config it
    auto config = i2s.defaultConfig(TX_MODE);
    config.pin_ws = I2C_WS;
    config.pin_bck = I2C_BCK;
    config.pin_data = I2C_DOUT;
    config.sample_rate = 48000;
    config.channels = 1;

    // to turn on the amp:
    // power->setPowerOutPut(AXP202_LDO4, true);

    // start
    i2s.begin(config);
}


void play_audio_task(void *pvParam)
{
    MemoryStream audio = *(MemoryStream *)pvParam;
    StreamCopy copier(out, audio);
    out.begin();
    while (audio)
    {
        copier.copy();
    }
    vTaskDelete(NULL);
}

void play_charging()
{
    xTaskCreate(play_audio_task, "Play Audio", 4096, &charging_wav, 0, NULL);
}
void set_audio(bool enabled)
{
    if (enabled)
    {
        power->setPowerOutPut(AXP202_LDO4, true);
    }
    else
    {
        power->setPowerOutPut(AXP202_LDO4, false);
    }
}

/* Init hardware */
void hardware_init()
{
    init_power();
    // start up the lcd backlight
    pinMode(TFT_BACKLIGHT, OUTPUT);
    // tft_espi not initialized because lvgl does it
    // start up everything else like the sensors
    init_touchpad();
    init_rtc();
    init_audio();

    // for checking if charging and button presses
    attachInterrupt(AXP202_INTERUPT, []
                    { axp_irq = true; }, FALLING);
}

/* Init LVGL */
void lvgl_init()
{
    lv_init();
    lv_display_t *disp;
    disp = lv_tft_espi_create(TFT_HEIGHT, TFT_WIDTH, draw_buf, sizeof(draw_buf));
    lv_display_set_rotation(disp, TFT_ROTATION);

    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
    lv_indev_set_read_cb(indev, hardware_touchpad_read);

    lv_tick_set_cb(lvgl_tick);

    lv_log_register_print_cb(lvgl_log_cb);

    // once this is done THEN turn on the backlight
    digitalWrite(TFT_BACKLIGHT, 1);
}
