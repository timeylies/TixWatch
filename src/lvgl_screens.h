//#include <lvgl.h>
// lvgl screens

lv_obj_t *homeScreen_timeText;
lv_obj_t *homeScreen_dateText;
lv_obj_t *homeScreen_appsButton;

void setup_homeScreen()
{
    lv_obj_t *homeScreen_topBar = lv_obj_create(lv_screen_active());
    lv_obj_align(homeScreen_topBar, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_size(homeScreen_topBar, TFT_WIDTH - 5, 30);

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

    lv_obj_t *homeScreen_appsButtonText = lv_label_create(homeScreen_appsButton);
    lv_label_set_text(homeScreen_appsButtonText, "Apps");
    lv_obj_set_style_text_font(homeScreen_appsButtonText, &lv_font_montserrat_18, 0);
    lv_obj_align(homeScreen_appsButtonText, LV_ALIGN_CENTER, 0, 0);
}