// Hardware stuff

TFT_eSPI *tft = nullptr;

//AXP stuff
AXP20X_Class *power;
void init_power(){
    power = new AXP20X_Class();
    Wire.begin(SENSOR_SDA, SENSOR_SCL);
    int ret = power->begin(Wire, AXP202_SLAVE_ADDRESS);
    if (ret == AXP_FAIL) {
        while (1) {
            Serial.println("AXP Power begin failed!");
            delay(1000);
        }
    }
    power->limitingOff();

    //Audio power is LDO4, keep it off for now
    power->setPowerOutPut(AXP202_LDO4, false);
    power->setLDO4Voltage(AXP202_LDO4_3300MV);
    // No use
    power->setPowerOutPut(AXP202_LDO3, false);
    //turn on lcd backlight
    power->setPowerOutPut(AXP202_LDO2, AXP202_ON);
}


/* Init hardware */
void hardware_init() {
    init_power();
    //start up the lcd
    pinMode(TFT_BACKLIGHT, OUTPUT);
    tft = new TFT_eSPI();
    tft->init();
    tft->setRotation(2);
    digitalWrite(TFT_BACKLIGHT, 1);
    //start up everything else like the sensors
}

