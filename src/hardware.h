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
}

// Audio stuff
AudioInfo info(48000, 2, 16);
I2SStream i2s;
InputMixer<int16_t> mixer;
//MemoryStream stream(plug, sizeof(plug));
StreamCopy copier(i2s, mixer);

void play_pluh(void *parameters)
{
    //initalize stuff
    MemoryStream stream(plug, sizeof(plug));
    mixer.add(stream);
    for(;;){
        //run copier here
        if(stream.available()){
            copier.copy();
        } else {
            stream.end();
            log_i("Pluh stream done, mixer size %i", mixer.size());
            vTaskDelete(NULL);
        }
        vTaskDelay(5);
    }
}

void init_audio()
{
    // config it
    auto config = i2s.defaultConfig(TX_MODE);
    config.pin_ws = I2C_WS;
    config.pin_bck = I2C_BCK;
    config.pin_data = I2C_DOUT;
    config.sample_rate = 48000;
    config.channels = 2;

    // turn on the amp
    power->setPowerOutPut(AXP202_LDO4, true);

    // start
    i2s.begin(config); 
    mixer.begin(info);

    //run the thingy
    xTaskCreate(play_pluh, "play pluh", 5000, NULL, 1, NULL);
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
    init_audio();
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

    // once this is done THEN turn on the backlight
    digitalWrite(TFT_BACKLIGHT, 1);
}
