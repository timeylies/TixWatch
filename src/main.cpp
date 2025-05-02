#include "config.h"
#include "hardware.h"

//to shut the errors up
#define TFT_WIDTH  240
#define TFT_HEIGHT 240

static uint32_t my_tick(void)
{
  return millis(); 
}

static void event_handler(lv_event_t *e)
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

bool irq = false;

void setup()
{
  // run the hardware setup first here
  hardware_init();
  lvgl_init();

  lv_tick_set_cb(my_tick);

  setup_homeScreen();
  lv_obj_add_event_cb(homeScreen_appsButton, event_handler, LV_EVENT_ALL, NULL);

  attachInterrupt(AXP202_INTERUPT, []
                  { irq = true; }, FALLING);
}

void loop()
{
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);           /* let this time pass */
  run_audio();
  if (irq)
  {
    irq = false;
    power->readIRQ();
    if (power->isPEKShortPressIRQ())
    {
      // show message box
      lv_obj_t *mbox = lv_msgbox_create(NULL);
      lv_obj_set_width(mbox, TFT_WIDTH - 10);
      lv_msgbox_add_text(mbox, "Power button pressed");
      lv_msgbox_add_close_button(mbox);
    }
    power->clearIRQ();
  }
}
