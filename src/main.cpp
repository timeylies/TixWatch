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

void update_homeScreen(DateTime now){
  //update time and date 
  if(now.hour() > 12){
    lv_label_set_text_fmt(homeScreen_timeText, "%i:%02i PM", now.hour() - 12, now.minute());
  } else {
    if(now.hour() == 0){
      lv_label_set_text_fmt(homeScreen_timeText, "%i:%02i AM", 12, now.minute());
    } else {
    lv_label_set_text_fmt(homeScreen_timeText, "%i:%02i AM", now.hour(), now.minute());
    }
  }
  lv_label_set_text_fmt(homeScreen_dateText, "%i-%i-%i", now.month(), now.day(), now.year());
  //log_i("Updated Home Screen");
}



void setup()
{
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);

  // run the hardware setup first here
  hardware_init();
  lvgl_init();

  setup_homeScreen();
  update_homeScreen(rtc.now());
  lv_obj_add_event_cb(homeScreen_appsButton, apps_event_handler, LV_EVENT_ALL, NULL);

  /* attachInterrupt(AXP202_INTERUPT, []
                  { irq = true; }, FALLING); */
}

//bool irq = false;

void loop()
{
  lv_timer_handler();
  delay(5);
  update_homeScreen(rtc.now());
  update_topBarBatteryStatus(power->isVBUSPlug(), power->getBattPercentage());
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
