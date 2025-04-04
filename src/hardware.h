// Screen stuff

TFT_eSPI *tft = nullptr;
#define DRAW_BUF_SIZE (TFT_WIDTH * TFT_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];
#define TFT_ROTATION  LV_DISPLAY_ROTATION_180

//Touch stuff

FocalTech_Class *touch;
TwoWire ti2c = TwoWire(1);

void init_touchpad(){
    ti2c.begin(FT6336_SDA, FT6336_SCL);
    touch = new FocalTech_Class;
    if(!touch->begin(ti2c)){
        Serial.println("Uh oh, no touchpad found?");
    }
}

void hardware_touchpad_read( lv_indev_t * indev, lv_indev_data_t * data )
{
    /*For example  ("my_..." functions needs to be implemented by you)
    int32_t x, y;
    bool touched = my_get_touch( &x, &y );

    if(!touched) {
        data->state = LV_INDEV_STATE_RELEASED;
    } else {
        data->state = LV_INDEV_STATE_PRESSED;

        data->point.x = x;
        data->point.y = y;
    }
     */
    uint16_t x, y;
    bool touched = touch->getTouched();
    touch->getPoint(x, y);

    if(!touched) {
        data->state = LV_INDEV_STATE_RELEASED;
    } else {
        data->state = LV_INDEV_STATE_PRESSED;

        data->point.x = x;
        data->point.y = y;
        Serial.printf("X: %i, Y: %i", x, y);
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
            Serial.println("AXP Power begin failed!");
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
}

/* Init hardware */
void hardware_init()
{
    init_power();
    // start up the lcd
    pinMode(TFT_BACKLIGHT, OUTPUT);
    tft = new TFT_eSPI();
    tft->init();
    digitalWrite(TFT_BACKLIGHT, 1);
    // start up everything else like the sensors
    init_touchpad();

}

/* Init LVGL */
void lvgl_init()
{
    lv_init();
    lv_display_t *disp;
    disp = lv_tft_espi_create(TFT_HEIGHT, TFT_WIDTH, draw_buf, sizeof(draw_buf));
    lv_display_set_rotation(disp, TFT_ROTATION);

    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
    lv_indev_set_read_cb(indev, hardware_touchpad_read);
}
