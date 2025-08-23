// Screen stuff

// to shut the errors up
#define TFT_WIDTH 240
#define TFT_HEIGHT 240

TFT_eSPI *tft = nullptr;
#define DRAW_BUF_SIZE (TFT_WIDTH * TFT_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];
#define TFT_ROTATION LV_DISPLAY_ROTATION_180

// Touch stuff

FocalTech_Class *touch;
TwoWire ti2c = TwoWire(1);

void init_touchpad()
{
    ti2c.begin(FT6336_SDA, FT6336_SCL, 100000);
    touch = new FocalTech_Class;
    if (!touch->begin(ti2c))
    {
        log_w("Uh oh, no touchpad found?");
    }
    else
    {
        log_i("Found touchpad!");
    }
}

// LVGL Stuff

void hardware_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    uint16_t x, y;
    bool touched = touch->getTouched();
    touch->getPoint(x, y);

    if (!touched)
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }
    else
    {
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
        log_v("%s", buf);
        break;
    case LV_LOG_LEVEL_INFO:
        log_i("%s", buf);
        break;
    case LV_LOG_LEVEL_WARN:
        log_w("%s", buf);
        break;
    case LV_LOG_LEVEL_ERROR:
        log_e("%s", buf);
        break;
    case LV_LOG_LEVEL_USER:
        log_i("%s", buf);
        break;
    }
}

// AXP stuff
AXP20X_Class *power;
void init_power()
{
    power = new AXP20X_Class();
    Wire.begin(SENSOR_SDA, SENSOR_SCL);
    int ret = power->begin(Wire, AXP202_SLAVE_ADDRESS);
    if (ret == AXP_FAIL)
    {
        while (1)
        {
            log_e("AXP Power begin failed!");
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
    power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ, true);
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
        log_w("Couldn't find RTC!");
        return;
    }
    else
    {
        log_i("Found RTC!");
    }
    if (rtc.lostPower())
    {
        log_i("RTC is NOT initialized, setting the time to compilation time...");
        // When time needs to be set on a new device, or after a power loss, the
        // following line sets the RTC to the date & time this sketch was compiled
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    rtc.start();
}

// Audio stuff
I2SStream i2s;

void init_audio()
{
    // config it
    auto config = i2s.defaultConfig(TX_MODE);
    config.pin_ws = I2C_WS;
    config.pin_bck = I2C_BCK;
    config.pin_data = I2C_DOUT;
    config.sample_rate = 48000;
    config.channels = 1;

    // turn on the amp
    power->setPowerOutPut(AXP202_LDO4, true);

    // start
    i2s.begin(config);
}

/* Init hardware */
void hardware_init()
{
    init_power();
    // start up the lcd
    pinMode(TFT_BACKLIGHT, OUTPUT);
    tft = new TFT_eSPI();
    tft->init();
    // start up everything else like the sensors
    init_touchpad();
    init_rtc();
    // init_audio();
    AudioLogger::instance().begin(Serial, AudioLogger::Info);
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
