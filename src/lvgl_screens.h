//#include <lvgl.h>
// lvgl screens

// to shut the errors up
#define TFT_WIDTH 240
#define TFT_HEIGHT 240

lv_obj_t *topBar;
lv_obj_t *topBar_batteryIcon;
lv_obj_t *topBar_batteryText;
lv_obj_t *topBar_wifiIcon;
lv_obj_t *topBar_soundIcon;

void setup_topBar(){
    topBar = lv_obj_create(lv_screen_active());
    lv_obj_align(topBar, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_size(topBar, TFT_WIDTH - 5, 30);
    lv_obj_clear_flag(topBar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_layout(topBar, LV_LAYOUT_FLEX);
    lv_obj_set_style_base_dir(topBar, LV_BASE_DIR_RTL, 0);
    lv_obj_set_flex_flow(topBar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(topBar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    topBar_batteryIcon = lv_label_create(topBar);
    lv_label_set_text(topBar_batteryIcon, LV_SYMBOL_BATTERY_EMPTY);

    topBar_batteryText = lv_label_create(topBar);
    lv_label_set_text(topBar_batteryText, "0%");
    lv_obj_set_style_text_font(topBar_batteryText, &lv_font_montserrat_16, 0);

    topBar_wifiIcon = lv_label_create(topBar);
    lv_label_set_text(topBar_wifiIcon, LV_SYMBOL_WIFI);

    topBar_soundIcon = lv_label_create(topBar);
    lv_label_set_text(topBar_soundIcon, LV_SYMBOL_VOLUME_MAX);
 
}

void update_topBarBatteryStatus(bool usbPluggedIn, int batPercent){
  if(usbPluggedIn){
    lv_label_set_text(topBar_batteryIcon, LV_SYMBOL_CHARGE);
  } else {
    int lvl = batPercent;
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
  lv_label_set_text_fmt(topBar_batteryText, "%i%%", batPercent);
}


lv_obj_t *homeScreen_timeText;
lv_obj_t *homeScreen_dateText;
lv_obj_t *homeScreen_appsButton;

void setup_homeScreen()
{
    setup_topBar();

    homeScreen_timeText = lv_label_create(lv_screen_active());
    lv_label_set_text(homeScreen_timeText, "12:00 AM");
    lv_obj_set_style_text_font(homeScreen_timeText, &lv_font_montserrat_42, 0);
    lv_obj_align(homeScreen_timeText, LV_ALIGN_CENTER, 0, 0);

    homeScreen_dateText = lv_label_create(lv_screen_active());
    lv_label_set_text(homeScreen_dateText, "12-30-2025");
    lv_obj_set_style_text_font(homeScreen_dateText, &lv_font_montserrat_16, 0);
    lv_obj_align(homeScreen_dateText, LV_ALIGN_CENTER, 0, 40);

    homeScreen_appsButton = lv_btn_create(lv_screen_active());
    lv_obj_align(homeScreen_appsButton, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_size(homeScreen_appsButton, 80, 40);
    lv_obj_set_ext_click_area(homeScreen_appsButton, 10); //helps on such a small screen

    lv_obj_t *homeScreen_appsButtonText = lv_label_create(homeScreen_appsButton);
    lv_label_set_text(homeScreen_appsButtonText, "Apps");
    lv_obj_set_style_text_font(homeScreen_appsButtonText, &lv_font_montserrat_18, 0);
    lv_obj_align(homeScreen_appsButtonText, LV_ALIGN_CENTER, 0, 0);
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