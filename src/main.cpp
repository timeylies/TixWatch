#include "config.h"
#include "hardware.h"

// to shut the errors up
#define TFT_WIDTH 240
#define TFT_HEIGHT 240

static void apps_event_handler(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED)
  {
    // show message box
    lv_obj_t *mbox = lv_msgbox_create(NULL);
    lv_obj_set_width(mbox, TFT_WIDTH - 10);
    lv_msgbox_add_text(mbox, "Apps isn't implemented yet");
    lv_msgbox_add_close_button(mbox);
  }
}

void update_homeScreen(){
  //update time and date 
  DateTime now = rtc.now();
  if(now.hour() > 12){
    lv_label_set_text_fmt(homeScreen_timeText, "%i:%02i PM", now.hour() - 12, now.minute());
  } else {
    lv_label_set_text_fmt(homeScreen_timeText, "%i:%02i AM", now.hour(), now.minute());
  }
  lv_label_set_text_fmt(homeScreen_dateText, "%i-%i-%i", now.month(), now.day(), now.year());
  //log_i("Updated Home Screen");
}

void update_topBar(){
  if(power->isVBUSPlug()){
    lv_label_set_text(topBar_batteryIcon, LV_SYMBOL_CHARGE);
  } else {
    int lvl = power->getBattPercentage();
    if(lvl > 85 && lvl < 100){
      lv_label_set_text(topBar_batteryIcon, LV_SYMBOL_BATTERY_FULL);
    }
    if(lvl > 75 && lvl < 85){
      lv_label_set_text(topBar_batteryIcon, LV_SYMBOL_BATTERY_3);
    }
    if(lvl > 50 && lvl < 75){
      lv_label_set_text(topBar_batteryIcon, LV_SYMBOL_BATTERY_2);
    }
    if(lvl > 25 && lvl < 50){
      lv_label_set_text(topBar_batteryIcon, LV_SYMBOL_BATTERY_1);
    }
    if(lvl >= 0 && lvl < 25){
      lv_label_set_text(topBar_batteryIcon, LV_SYMBOL_BATTERY_EMPTY);
    }
  }
  lv_label_set_text_fmt(topBar_batteryText, "%i%%", power->getBattPercentage());
  //lv_obj_align_to(topBar_batteryText, topBar_batteryIcon, LV_ALIGN_OUT_LEFT_MID, -5, 0);
}

void setup()
{
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);

  // run the hardware setup first here
  hardware_init();
  lvgl_init();

  setup_homeScreen();
  update_homeScreen();
  lv_obj_add_event_cb(homeScreen_appsButton, apps_event_handler, LV_EVENT_ALL, NULL);

  /* attachInterrupt(AXP202_INTERUPT, []
                  { irq = true; }, FALLING); */
}

//bool irq = false;

void loop()
{
  lv_timer_handler();
  delay(5);
  update_homeScreen();
  update_topBar();
  /*
  if (irq)
  {
    irq = false;
    power->readIRQ();
    if (power->isPEKShortPressIRQ())
    {
      // show message box
      // lv_obj_t *mbox = lv_msgbox_create(NULL);
      // lv_obj_set_width(mbox, TFT_WIDTH - 10);
      // lv_msgbox_add_text(mbox, "Power button pressed");
      // lv_msgbox_add_close_button(mbox);
    }
    power->clearIRQ();
  }
  */
}
