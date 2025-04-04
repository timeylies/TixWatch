#include "config.h"
#include "hardware.h"

void setup()
{
  Serial.begin(115200);
  // run the hardware setup first here
  hardware_init();
  lvgl_init();

  lv_obj_t *btn1 = lv_button_create(lv_screen_active());
  lv_obj_align(btn1, LV_ALIGN_CENTER, 0, 0);
  lv_obj_remove_flag(btn1, LV_OBJ_FLAG_PRESS_LOCK);

  lv_obj_t *label = lv_label_create(btn1);
  lv_label_set_text(label, "Button");
  lv_obj_center(label);
}

void loop()
{
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);           /* let this time pass */
}
