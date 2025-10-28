#include "config.h"
#include "hardware.h"

// to shut the errors up
#define TFT_WIDTH 240
#define TFT_HEIGHT 240

bool irq = false;

void setup()
{
  Serial.begin(115200);

  // run the hardware setup first here
  hardware_init();
  lvgl_init();

  setup_screens();
  lv_screen_load(homeScreen);

  lv_obj_add_event_cb(topBar, topBar_swipe_cb, LV_EVENT_GESTURE, NULL);
  lv_obj_add_event_cb(controlPanel_closeButton, controlPanel_close_cb, LV_EVENT_CLICKED, NULL);

  attachInterrupt(AXP202_INTERUPT, []
                  { irq = true; }, FALLING);
}

void loop()
{
  lv_timer_handler();
  delay(5);
  update_homeScreen(rtc.now());
  update_topBarBatteryStatus(power->isVBUSPlug(), power->getBattPercentage());

  //Use side button as back button
  if (irq)
  {
    irq = false;
    power->readIRQ();
    if (power->isPEKShortPressIRQ())
    {
      if (lv_screen_active() == homeScreen)
      {
        // TODO: Put watch into sleep when in homescreen
      }
      if (lv_screen_active() == appsScreen)
      {
        // load home screen
        lv_obj_set_parent(topBar, homeScreen);
        lv_obj_set_parent(controlPanel, homeScreen);
        lv_screen_load_anim(homeScreen, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
      }
      if (lv_screen_active() == settings_app)
      {
        // load home screen
        lv_obj_set_parent(topBar, appsScreen);
        lv_obj_set_parent(controlPanel, appsScreen);
        lv_screen_load_anim(appsScreen, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
      }
    }
    power->clearIRQ();
  }
}
