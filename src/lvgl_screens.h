#pragma once
#include <lvgl.h>
#include <iostream>

// to shut the errors up
#define TFT_WIDTH 240
#define TFT_HEIGHT 240

enum TopBarControlPanelState {
    topBarHidden,
    topBarShowing,
    controlPanelShowing,
    playingAnim
};
TopBarControlPanelState topBarControlPanelState = topBarShowing;

lv_timer_t* topBarTimer;
lv_anim_timeline_t* topbarControlPanelAnimTimeline;

lv_obj_t* topBar;
lv_obj_t* topBar_batteryIcon;
lv_obj_t* topBar_batteryText;
lv_obj_t* topBar_wifiIcon;
lv_obj_t* topBar_soundIcon;

void setup_topBar(lv_obj_t* screen) {
    static lv_style_t style;
    lv_style_init(&style);

    lv_style_set_bg_opa(&style, LV_OPA_70);

    topBar = lv_obj_create(screen);

    lv_obj_add_style(topBar, &style, 0);
    lv_obj_align(topBar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_size(topBar, lv_display_get_horizontal_resolution(NULL) - 5, 30);
    lv_obj_remove_flag(topBar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(topBar, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_layout(topBar, LV_LAYOUT_FLEX);
    lv_obj_set_style_base_dir(topBar, LV_BASE_DIR_RTL, 0);
    lv_obj_set_flex_flow(topBar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(topBar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    topBar_batteryIcon = lv_label_create(topBar);
    lv_label_set_text(topBar_batteryIcon, LV_SYMBOL_BATTERY_EMPTY);

    topBar_batteryText = lv_label_create(topBar);
    lv_label_set_text(topBar_batteryText, "67%");
    lv_obj_set_style_text_font(topBar_batteryText, &lv_font_montserrat_16, 0);

    topBar_wifiIcon = lv_label_create(topBar);
    lv_label_set_text(topBar_wifiIcon, LV_SYMBOL_WIFI);
    lv_obj_add_flag(topBar_wifiIcon, LV_OBJ_FLAG_HIDDEN);

    topBar_soundIcon = lv_label_create(topBar);
    lv_label_set_text(topBar_soundIcon, LV_SYMBOL_VOLUME_MAX);
    lv_obj_add_flag(topBar_soundIcon, LV_OBJ_FLAG_HIDDEN);

}

void update_topBarBatteryStatus(bool usbPluggedIn, int batPercent) {
    if (usbPluggedIn) {
        lv_label_set_text(topBar_batteryIcon, LV_SYMBOL_CHARGE);
    } else {
        // Static const array stays in Flash (no RAM usage)
        static const char* icons[] = {LV_SYMBOL_BATTERY_EMPTY, LV_SYMBOL_BATTERY_1, LV_SYMBOL_BATTERY_2, LV_SYMBOL_BATTERY_3, LV_SYMBOL_BATTERY_FULL};
        int idx = (batPercent < 25) ? 0 : (batPercent < 50) ? 1 : (batPercent < 75) ? 2 : (batPercent < 85) ? 3 : 4;
        lv_label_set_text(topBar_batteryIcon, icons[idx]);
    }
    lv_label_set_text_fmt(topBar_batteryText, "%i%%", batPercent);
}

lv_obj_t* controlPanel;
lv_obj_t* controlPanel_wifiButton;
lv_obj_t* controlPanel_soundButton;
lv_obj_t* controlPanel_brightnessSlider;
lv_obj_t* controlPanel_volumeSlider;
lv_obj_t* controlPanel_closeButton;

static void controlPanel_wifiButton_event_handler(lv_event_t* e);
static void controlPanel_soundButton_event_handler(lv_event_t* e);
static void controlPanel_brightnessSlider_event_handler(lv_event_t* e);
static void controlPanel_volumeSlider_event_handler(lv_event_t* e);

void setup_controlPanel(lv_obj_t* screen) {
    controlPanel = lv_obj_create(screen);
    lv_obj_align(controlPanel, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_size(controlPanel, lv_display_get_horizontal_resolution(NULL) - 5, 150);
    lv_obj_remove_flag(controlPanel, LV_OBJ_FLAG_SCROLLABLE);

    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_color(&style, lv_palette_darken(LV_PALETTE_GREY, 3));

    static lv_style_t style_tg;
    lv_style_init(&style_tg);
    lv_style_set_bg_color(&style_tg, lv_palette_main(LV_PALETTE_BLUE));

    controlPanel_wifiButton = lv_btn_create(controlPanel);
    lv_obj_set_size(controlPanel_wifiButton, LV_PCT(48), LV_SIZE_CONTENT);
    lv_obj_align(controlPanel_wifiButton, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_add_flag(controlPanel_wifiButton, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_style(controlPanel_wifiButton, &style, 0);
    lv_obj_add_style(controlPanel_wifiButton, &style_tg, LV_STATE_CHECKED);
    lv_obj_add_event_cb(controlPanel_wifiButton, controlPanel_wifiButton_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t* controlPanel_wifiButtonIcon = lv_label_create(controlPanel_wifiButton);
    lv_label_set_text(controlPanel_wifiButtonIcon, LV_SYMBOL_WIFI " Wi-Fi");
    lv_obj_align(controlPanel_wifiButtonIcon, LV_ALIGN_CENTER, 0, 0);

    controlPanel_soundButton = lv_btn_create(controlPanel);
    lv_obj_set_size(controlPanel_soundButton, LV_PCT(48), LV_SIZE_CONTENT);
    lv_obj_align(controlPanel_soundButton, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_flag(controlPanel_soundButton, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_style(controlPanel_soundButton, &style, 0);
    lv_obj_add_style(controlPanel_soundButton, &style_tg, LV_STATE_CHECKED);
    lv_obj_add_event_cb(controlPanel_soundButton, controlPanel_soundButton_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t* controlPanel_soundButtonIcon = lv_label_create(controlPanel_soundButton);
    lv_label_set_text(controlPanel_soundButtonIcon, LV_SYMBOL_AUDIO " Sound");
    lv_obj_align(controlPanel_soundButtonIcon, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* controlPanel_brightnessSliderIcon = lv_label_create(controlPanel);
    lv_label_set_text(controlPanel_brightnessSliderIcon, LV_SYMBOL_IMAGE);
    lv_obj_set_style_text_font(controlPanel_brightnessSliderIcon, &lv_font_montserrat_18, 0);
    lv_obj_align_to(controlPanel_brightnessSliderIcon, controlPanel_wifiButton, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
    lv_obj_set_x(controlPanel_brightnessSliderIcon, LV_PCT(1));

    controlPanel_brightnessSlider = lv_slider_create(controlPanel);
    lv_obj_align_to(controlPanel_brightnessSlider, controlPanel_brightnessSliderIcon, LV_ALIGN_OUT_RIGHT_MID, 15, 1);
    lv_obj_set_size(controlPanel_brightnessSlider, LV_PCT(80), 10);
    lv_obj_add_event_cb(controlPanel_brightnessSlider, controlPanel_brightnessSlider_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t* controlPanel_volumeSliderIcon = lv_label_create(controlPanel);
    lv_label_set_text(controlPanel_volumeSliderIcon, LV_SYMBOL_VOLUME_MAX);
    lv_obj_align_to(controlPanel_volumeSliderIcon, controlPanel_brightnessSliderIcon, LV_ALIGN_OUT_BOTTOM_MID, 0, 13);

    controlPanel_volumeSlider = lv_slider_create(controlPanel);
    lv_obj_align_to(controlPanel_volumeSlider, controlPanel_volumeSliderIcon, LV_ALIGN_OUT_RIGHT_MID, 16, 1);
    lv_obj_set_size(controlPanel_volumeSlider, LV_PCT(80), 10);
    lv_obj_add_event_cb(controlPanel_volumeSlider, controlPanel_volumeSlider_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    controlPanel_closeButton = lv_btn_create(controlPanel);
    lv_obj_align(controlPanel_closeButton, LV_ALIGN_BOTTOM_MID, 0, 7);
    lv_obj_set_size(controlPanel_closeButton, LV_PCT(100), 20);
    lv_obj_set_style_bg_color(controlPanel_closeButton, lv_color_hex(0x303030), 0);
    lv_obj_set_ext_click_area(controlPanel_closeButton, 10); // helps on such a small screen

    lv_obj_t* controlPanel_closeButtonLabel = lv_label_create(controlPanel_closeButton);
    lv_obj_center(controlPanel_closeButtonLabel);
    lv_label_set_text(controlPanel_closeButtonLabel, LV_SYMBOL_UP);

}

//animations for topbar and control panel

void anim_y_cb(void* var, int32_t v)
{
    lv_obj_set_y((lv_obj_t*)var, v);
}


void showControlPanel() {
    if (topbarControlPanelAnimTimeline != NULL) {
        lv_anim_timeline_delete(topbarControlPanelAnimTimeline); //delete to stop last animation
    }
    topbarControlPanelAnimTimeline = lv_anim_timeline_create();

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, topBar);
    lv_anim_set_values(&a, lv_obj_get_y(topBar), -lv_obj_get_height(topBar));
    lv_anim_set_duration(&a, 400);
    lv_anim_set_exec_cb(&a, anim_y_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
    lv_anim_timeline_add(topbarControlPanelAnimTimeline, 0, &a);

    lv_anim_t a1;
    lv_anim_init(&a1);
    lv_anim_set_var(&a1, controlPanel);
    lv_anim_set_values(&a1, -lv_obj_get_height(controlPanel), 0);
    lv_anim_set_duration(&a1, 800);
    lv_anim_set_exec_cb(&a1, anim_y_cb);
    lv_anim_set_path_cb(&a1, lv_anim_path_ease_out);
    lv_anim_set_completed_cb(&a1, [](lv_anim_t* anim) {
        topBarControlPanelState = controlPanelShowing;
        });
    lv_anim_timeline_add(topbarControlPanelAnimTimeline, 300, &a1);

    lv_timer_pause(topBarTimer);

    topBarControlPanelState = playingAnim;
    lv_anim_timeline_start(topbarControlPanelAnimTimeline);
}

void hideControlPanel() {
    if (topbarControlPanelAnimTimeline != NULL) {
        lv_anim_timeline_delete(topbarControlPanelAnimTimeline); //delete to stop last animation
    }
    topbarControlPanelAnimTimeline = lv_anim_timeline_create();

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, topBar);
    lv_anim_set_values(&a, -lv_obj_get_height(topBar), 0);
    lv_anim_set_duration(&a, 600);
    lv_anim_set_exec_cb(&a, anim_y_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_set_completed_cb(&a, [](lv_anim_t* anim) {
        topBarControlPanelState = topBarShowing;
        });
    lv_anim_timeline_add(topbarControlPanelAnimTimeline, 300, &a);

    lv_anim_t a1;
    lv_anim_init(&a1);
    lv_anim_set_var(&a1, controlPanel);
    lv_anim_set_values(&a1, lv_obj_get_y(controlPanel), -lv_obj_get_height(controlPanel));
    lv_anim_set_duration(&a1, 500);
    lv_anim_set_exec_cb(&a1, anim_y_cb);
    lv_anim_set_path_cb(&a1, lv_anim_path_ease_in);
    lv_anim_timeline_add(topbarControlPanelAnimTimeline, 0, &a1);

    lv_timer_reset(topBarTimer);
    lv_timer_resume(topBarTimer);

    topBarControlPanelState = playingAnim;
    lv_anim_timeline_start(topbarControlPanelAnimTimeline);
}

void showTopBar() {
    if (topBarControlPanelState != topBarShowing) {

        if (topbarControlPanelAnimTimeline != NULL) {
            lv_anim_timeline_delete(topbarControlPanelAnimTimeline); //delete to stop last animation
        }
        topbarControlPanelAnimTimeline = lv_anim_timeline_create();

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, topBar);
        lv_anim_set_values(&a, lv_obj_get_y(topBar), 0);
        lv_anim_set_duration(&a, 600);
        lv_anim_set_exec_cb(&a, anim_y_cb);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
        lv_anim_set_completed_cb(&a, [](lv_anim_t* anim) {
            topBarControlPanelState = topBarShowing;
            });
        lv_anim_timeline_add(topbarControlPanelAnimTimeline, 0, &a);

        topBarControlPanelState = playingAnim;
        lv_anim_timeline_start(topbarControlPanelAnimTimeline);
    }

    lv_timer_reset(topBarTimer);
}
void hideTopBar() {
    if (topBarControlPanelState != topBarHidden) {
        if (topbarControlPanelAnimTimeline != NULL) {
            lv_anim_timeline_delete(topbarControlPanelAnimTimeline); //delete to stop last animation
        }
        topbarControlPanelAnimTimeline = lv_anim_timeline_create();

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, topBar);
        lv_anim_set_values(&a, lv_obj_get_y(topBar), -lv_obj_get_height(topBar));
        lv_anim_set_duration(&a, 200);
        lv_anim_set_exec_cb(&a, anim_y_cb);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
        lv_anim_set_completed_cb(&a, [](lv_anim_t* anim) {
            topBarControlPanelState = topBarHidden;
            });
        lv_anim_timeline_add(topbarControlPanelAnimTimeline, 0, &a);

        topBarControlPanelState = playingAnim;
        lv_anim_timeline_start(topbarControlPanelAnimTimeline);
    }
}

void setupTopBarTimer() {
    topBarTimer = lv_timer_create([](lv_timer_t* timer) {
        if (topBarControlPanelState != controlPanelShowing && topBarControlPanelState != topBarHidden) {
            hideTopBar();
        }
        }, 5000, NULL); //TODO: Allow duration adjustment in settings
}


void topBar_swipe_cb(lv_event_t* event)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
    if (dir == LV_DIR_BOTTOM) {
        // show control panel and hide top bar
        showControlPanel();
    }
}

void controlPanel_close_cb(lv_event_t* event) {
    hideControlPanel();
}

lv_obj_t* appsScreen;
lv_obj_t* appsScreen_cont;

void appsScreen_swipe_cb(lv_event_t* event)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
    if (dir == LV_DIR_BOTTOM) {
        // show control panel and hide top bar
        showTopBar();
    }
}

void scroll_top_value_event_cb(lv_event_t* e) {
    lv_obj_t* scr = lv_event_get_target_obj(e);
    int scroll_value = lv_obj_get_scroll_top(scr);
    if (scroll_value <= 0) {
        if (topBarControlPanelState == topBarHidden) {
            showTopBar();
        }
    }
    else {
        if (topBarControlPanelState != topBarHidden && topBarControlPanelState != playingAnim) {
            hideTopBar();
            topBarControlPanelState = topBarHidden; //so it doesnt run the anim again
        }
    }
}

void setup_appsScreen() {
    appsScreen = lv_obj_create(NULL);
    lv_obj_add_event_cb(appsScreen, appsScreen_swipe_cb, LV_EVENT_GESTURE, NULL);

    appsScreen_cont = lv_obj_create(appsScreen);
    lv_obj_set_size(appsScreen_cont, 240, 240);
    lv_obj_align(appsScreen_cont, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_flex_flow(appsScreen_cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_remove_flag(appsScreen_cont, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_add_event_cb(appsScreen_cont, scroll_top_value_event_cb, LV_EVENT_SCROLL, NULL);

    //buttons should be 64x64 in size for "perfect" spacing
}

lv_obj_t* homeScreen;
lv_obj_t* homeScreen_timeText;
lv_obj_t* homeScreen_dateText;
lv_obj_t* homeScreen_appsButton;
bool is24HourFormat = false;

void homeScreen_swipe_cb(lv_event_t* event)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
    if (dir == LV_DIR_BOTTOM) {
        // show control panel and hide top bar
        showTopBar();
    }
}

void appsButton_event_cb(lv_event_t* event) {
    if (lv_event_get_target(event) == homeScreen_appsButton) {
        //top bar and control panel
        //lv_obj_set_parent(topBar, appsScreen);
        //lv_obj_set_parent(controlPanel, appsScreen);
        lv_screen_load_anim(appsScreen, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
    }
}


void setup_homeScreen()
{
    homeScreen_timeText = lv_label_create(homeScreen);
    lv_label_set_text(homeScreen_timeText, "12:00 AM");
    lv_obj_set_style_text_font(homeScreen_timeText, &lv_font_montserrat_42, 0);
    lv_obj_align(homeScreen_timeText, LV_ALIGN_CENTER, 0, 0);

    homeScreen_dateText = lv_label_create(homeScreen);
    lv_label_set_text(homeScreen_dateText, "12-30-2025");
    lv_obj_set_style_text_font(homeScreen_dateText, &lv_font_montserrat_16, 0);
    lv_obj_align(homeScreen_dateText, LV_ALIGN_CENTER, 0, 40);

    homeScreen_appsButton = lv_btn_create(homeScreen);
    lv_obj_align(homeScreen_appsButton, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_size(homeScreen_appsButton, 80, 40);
    lv_obj_set_ext_click_area(homeScreen_appsButton, 10); //helps on such a small screen
    lv_obj_add_event_cb(homeScreen_appsButton, appsButton_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* homeScreen_appsButtonText = lv_label_create(homeScreen_appsButton);
    lv_label_set_text(homeScreen_appsButtonText, "Apps");
    lv_obj_set_style_text_font(homeScreen_appsButtonText, &lv_font_montserrat_18, 0);
    lv_obj_align(homeScreen_appsButtonText, LV_ALIGN_CENTER, 0, 0);

    lv_obj_add_event_cb(homeScreen, homeScreen_swipe_cb, LV_EVENT_GESTURE, NULL);

}


//no need for a screen (object that contains everything)

void homeButton_event_cb(lv_event_t* event) {

    //lv_obj_set_parent(topBar, homeScreen);
    //lv_obj_set_parent(controlPanel, homeScreen);
    lv_screen_load_anim(homeScreen, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
}

void setup_homeButtonApp() {
    lv_obj_t* obj = lv_btn_create(appsScreen_cont);
    lv_obj_set_size(obj, 64, 64);
    lv_obj_add_event_cb(obj, homeButton_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* label = lv_label_create(obj);
    lv_label_set_text(label, LV_SYMBOL_HOME);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_42, 0);
    lv_obj_center(label);
}

void settingsBackButton_event_cb(lv_event_t* event) {
    lv_obj_t* obj = lv_event_get_target_obj(event);
    lv_obj_t* menu = (lv_obj_t*)lv_event_get_user_data(event);

    if (lv_menu_back_button_is_root(menu, obj)) {
        //lv_obj_set_parent(topBar, appsScreen);
        //lv_obj_set_parent(controlPanel, appsScreen);
        lv_screen_load_anim(appsScreen, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
    }
}

lv_obj_t* settings_app;


void settingsButton_event_cb(lv_event_t* event) {
    //lv_obj_set_parent(topBar, settings_app);
    //lv_obj_set_parent(controlPanel, settings_app);
    lv_screen_load_anim(settings_app, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
    hideTopBar();
}

typedef enum {
    LV_MENU_ITEM_BUILDER_VARIANT_1,
    LV_MENU_ITEM_BUILDER_VARIANT_2
} lv_menu_builder_variant_t;

static lv_obj_t* create_text(lv_obj_t* parent, const char* icon, const char* txt,
    lv_menu_builder_variant_t builder_variant)
{
    lv_obj_t* obj = lv_menu_cont_create(parent);

    lv_obj_t* img = NULL;
    lv_obj_t* label = NULL;

    if (icon) {
        img = lv_image_create(obj);
        lv_image_set_src(img, icon);
    }

    if (txt) {
        label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
        lv_obj_set_flex_grow(label, 1);
    }

    if (builder_variant == LV_MENU_ITEM_BUILDER_VARIANT_2 && icon && txt) {
        lv_obj_add_flag(img, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        lv_obj_swap(img, label);
    }

    return obj;
}

static lv_obj_t* create_slider(lv_obj_t* parent, const char* icon, const char* txt, int32_t min, int32_t max,
    int32_t val)
{
    lv_obj_t* obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t* slider = lv_slider_create(obj);
    lv_obj_set_flex_grow(slider, 1);
    lv_slider_set_range(slider, min, max);
    lv_slider_set_value(slider, val, LV_ANIM_OFF);

    if (icon == NULL) {
        lv_obj_add_flag(slider, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    }

    return slider;
}

static lv_obj_t* create_switch(lv_obj_t* parent, const char* icon, const char* txt, bool chk)
{
    lv_obj_t* obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);

    lv_obj_t* sw = lv_switch_create(obj);
    lv_obj_add_state(sw, chk ? LV_STATE_CHECKED : LV_STATE_DEFAULT);

    return sw;
}
//wifi stuff
lv_obj_t* settings_wifi_switch;
lv_obj_t* settings_wifi_scan_btn;
lv_obj_t* settings_wifi_scan_btn_text;
lv_obj_t* settings_wifi_list;

static void settings_wifi_switch_event_handler(lv_event_t* e);
static void settings_wifi_scan_btn_event_handler(lv_event_t* e);

//sound stuff
lv_obj_t* settings_sound_switch;
lv_obj_t* settings_sound_volume_slider;

static void settings_sound_switch_event_handler(lv_event_t* e);
static void settings_sound_volume_slider_event_handler(lv_event_t* e);

//display stuff
lv_obj_t* settings_display_brightness_slider;
lv_obj_t* settings_display_timeout_roller;

static void settings_display_brightness_slider_event_handler(lv_event_t* e);
static void settings_display_timeout_roller_event_handler(lv_event_t* e);

//time and date stuff
lv_obj_t* settings_timeAndDate_hrRoller;
lv_obj_t* settings_timeAndDate_minRoller;
lv_obj_t* settings_timeAndDate_amPmRoller;
lv_obj_t* settings_timeAndDate_calendar;
lv_obj_t* settings_timeAndDate_setBtn;
lv_obj_t* settings_timeAndDate_24hrSwitch;
lv_obj_t* settings_timeAndDate_setNtpSwitch;
lv_obj_t* settings_timeAndDate_syncTimeAndDateBtn;

lv_calendar_date_t settings_timeAndDate_selectedDate;

static void settings_timeAndDate_calendar_event_handler(lv_event_t* e);
static void settings_timeAndDate_setBtn_event_handler(lv_event_t* e);
static void settings_timeAndDate_24hrSwitch_event_handler(lv_event_t* e);
static void settings_timeAndDate_setNtpSwitch_event_handler(lv_event_t* e);
static void settings_timeAndDate_syncTimeAndDateBtn_event_handler(lv_event_t* e);

const char* settings_timeAndDate_hrRoller_12hr = "12\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11";
const char* settings_timeAndDate_hrRoller_24hr = "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24";
const char* settings_timeAndDate_minRoller_text = "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59";

//update stuff
lv_obj_t* settings_updates_checkForUpdatesBtn;
lv_obj_t* settings_updates_updateStatusLabel;
lv_obj_t* settings_updates_startUpdateBtn;

static void settings_updates_checkForUpdatesBtn_event_handler(lv_event_t* e);
static void settings_updates_startUpdateBtn_event_handler(lv_event_t* e);

void setup_settings() {
    lv_obj_t* menu = lv_menu_create(settings_app);
    lv_obj_set_size(menu, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL));

    lv_color_t bg_color = lv_obj_get_style_bg_color(menu, LV_PART_MAIN);
    if (lv_color_brightness(bg_color) > 127) {
        lv_obj_set_style_bg_color(menu, lv_color_darken(lv_obj_get_style_bg_color(menu, LV_PART_MAIN), 10), 0);
    }
    else {
        lv_obj_set_style_bg_color(menu, lv_color_darken(lv_obj_get_style_bg_color(menu, LV_PART_MAIN), 50), 0);
    }

    lv_menu_set_mode_root_back_button(menu, LV_MENU_ROOT_BACK_BUTTON_ENABLED);
    lv_obj_add_event_cb(menu, settingsBackButton_event_cb, LV_EVENT_CLICKED, menu);

    lv_obj_t* menu_header = lv_menu_get_main_header(menu);
    lv_obj_set_height(menu_header, 30);
    lv_obj_center(menu);
    lv_obj_set_ext_click_area(lv_menu_get_main_header_back_button(menu), 20); //helps on such a small screen

    lv_obj_t* cont;
    lv_obj_t* section;

    /*Create sub pages*/
    //Wifi Page
    lv_obj_t* sub_wifi_page = lv_menu_page_create(menu, "Wi-Fi");
    lv_obj_set_style_pad_hor(sub_wifi_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), LV_PART_MAIN), 0);
    lv_menu_separator_create(sub_wifi_page);
    section = lv_menu_section_create(sub_wifi_page);
    settings_wifi_switch = create_switch(section, LV_SYMBOL_WIFI, "Wi-Fi", false);
    lv_obj_add_event_cb(settings_wifi_switch, settings_wifi_switch_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_menu_separator_create(sub_wifi_page);

    section = lv_menu_section_create(sub_wifi_page);
    lv_obj_set_flex_align(section, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_t* wifi_top_btn_seperator = lv_menu_separator_create(section);
    lv_obj_set_height(wifi_top_btn_seperator, 10);
    settings_wifi_scan_btn = lv_button_create(section);
    lv_obj_set_size(settings_wifi_scan_btn, LV_PCT(90), 30);
    settings_wifi_scan_btn_text = lv_label_create(settings_wifi_scan_btn);
    lv_label_set_text(settings_wifi_scan_btn_text, LV_SYMBOL_REFRESH " Scan");
    lv_obj_center(settings_wifi_scan_btn_text);

    lv_obj_t* wifi_btn_list_seperator = lv_menu_separator_create(section);
    lv_obj_set_height(wifi_btn_list_seperator, 10);
    lv_obj_add_event_cb(settings_wifi_scan_btn, settings_wifi_scan_btn_event_handler, LV_EVENT_CLICKED, NULL);

    settings_wifi_list = lv_list_create(section);
    lv_obj_set_size(settings_wifi_list, LV_PCT(90), LV_SIZE_CONTENT);
    lv_obj_center(settings_wifi_list);

    /*Add a button to the list*/
    lv_obj_t* btn;
    btn = lv_list_add_button(settings_wifi_list, LV_SYMBOL_OK, "Press Scan...");

    //Sound Page
    lv_obj_t* sub_sound_page = lv_menu_page_create(menu, "Sound");
    lv_obj_set_style_pad_hor(sub_sound_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), LV_PART_MAIN), 0);
    lv_menu_separator_create(sub_sound_page);
    section = lv_menu_section_create(sub_sound_page);

    settings_sound_switch = create_switch(section, LV_SYMBOL_AUDIO, "Sound", false);
    lv_obj_add_event_cb(settings_sound_switch, settings_sound_switch_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_menu_separator_create(sub_sound_page);

    section = lv_menu_section_create(sub_sound_page);
    settings_sound_volume_slider = create_slider(section, LV_SYMBOL_VOLUME_MAX, "Volume", 0, 100, 100);
    lv_obj_add_event_cb(settings_sound_volume_slider, settings_sound_volume_slider_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t* sub_display_page = lv_menu_page_create(menu, "Display");
    lv_obj_set_style_pad_hor(sub_display_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), LV_PART_MAIN), 0);
    lv_menu_separator_create(sub_display_page);
    section = lv_menu_section_create(sub_display_page);
    settings_display_brightness_slider = create_slider(section, LV_SYMBOL_IMAGE, "Brightness", 0, 100, 100);
    lv_obj_add_event_cb(settings_display_brightness_slider, settings_display_brightness_slider_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_menu_separator_create(sub_display_page);

    section = lv_menu_section_create(sub_display_page);
    lv_obj_t* display_timeout_text = create_text(section, NULL, "Screen Timeout", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_separator_create(sub_display_page);
    lv_obj_set_flex_align(section, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    settings_display_timeout_roller = lv_roller_create(section);
    lv_roller_set_options(settings_display_timeout_roller,
        "5s\n" //0
        "10s\n" //1
        "15s\n" //2
        "30s\n" //3
        "1m\n" //4
        "5m\n" //5
        "Never", //6
        LV_ROLLER_MODE_NORMAL);
    lv_obj_add_event_cb(settings_display_timeout_roller, settings_display_timeout_roller_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_roller_set_visible_row_count(settings_display_timeout_roller, 3);
    lv_obj_set_width(settings_display_timeout_roller, LV_PCT(80));
    lv_menu_separator_create(section);

    //time and date page
    lv_obj_t* sub_timeAndDate_page = lv_menu_page_create(menu, "Time & Date");
    lv_obj_set_style_pad_hor(sub_timeAndDate_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), LV_PART_MAIN), 0);
    lv_menu_separator_create(sub_timeAndDate_page);

    lv_obj_t* timeAndDateSet_section = lv_menu_section_create(sub_timeAndDate_page);
    lv_obj_set_height(timeAndDateSet_section, lv_pct(80));
    lv_obj_set_flex_flow(timeAndDateSet_section, LV_FLEX_FLOW_ROW);
    settings_timeAndDate_hrRoller = lv_roller_create(timeAndDateSet_section);
    lv_roller_set_options(settings_timeAndDate_hrRoller, settings_timeAndDate_hrRoller_12hr, LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(settings_timeAndDate_hrRoller, 4);
    lv_obj_set_width(settings_timeAndDate_hrRoller, lv_pct(33));
    lv_obj_set_height(settings_timeAndDate_hrRoller, lv_pct(100));

    settings_timeAndDate_minRoller = lv_roller_create(timeAndDateSet_section);
    lv_roller_set_options(settings_timeAndDate_minRoller, settings_timeAndDate_minRoller_text, LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(settings_timeAndDate_minRoller, 4);
    lv_obj_set_width(settings_timeAndDate_minRoller, lv_pct(33));
    lv_obj_set_height(settings_timeAndDate_minRoller, lv_pct(100));

    settings_timeAndDate_amPmRoller = lv_roller_create(timeAndDateSet_section);
    lv_roller_set_options(settings_timeAndDate_amPmRoller, "PM\n" "AM", LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(settings_timeAndDate_amPmRoller, 4);
    lv_obj_set_width(settings_timeAndDate_amPmRoller, lv_pct(33));
    lv_obj_set_height(settings_timeAndDate_amPmRoller, lv_pct(100));

    lv_menu_separator_create(sub_timeAndDate_page);

    section = lv_menu_section_create(sub_timeAndDate_page);
    settings_timeAndDate_calendar = lv_calendar_create(section);
    lv_obj_set_size(settings_timeAndDate_calendar, LV_PCT(100), 250);
    lv_calendar_set_today_date(settings_timeAndDate_calendar, 2026, 01, 01);
    lv_calendar_set_month_shown(settings_timeAndDate_calendar, 2026, 01);
    lv_calendar_add_header_arrow(settings_timeAndDate_calendar);
    lv_obj_add_event_cb(settings_timeAndDate_calendar, settings_timeAndDate_calendar_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_menu_separator_create(sub_timeAndDate_page);

    section = lv_menu_section_create(sub_timeAndDate_page);
    settings_timeAndDate_setBtn = lv_button_create(section);
    lv_obj_set_width(settings_timeAndDate_setBtn, LV_PCT(100));
    lv_obj_t* setBtn_label = lv_label_create(settings_timeAndDate_setBtn);
    lv_obj_center(setBtn_label);
    lv_label_set_text(setBtn_label, "Set Time & Date");
    lv_obj_add_event_cb(settings_timeAndDate_setBtn, settings_timeAndDate_setBtn_event_handler, LV_EVENT_CLICKED, NULL);

    lv_menu_separator_create(sub_timeAndDate_page);

    section = lv_menu_section_create(sub_timeAndDate_page);
    settings_timeAndDate_24hrSwitch = create_switch(section, NULL, "24-Hour Format", false);
    lv_obj_add_event_cb(settings_timeAndDate_24hrSwitch, settings_timeAndDate_24hrSwitch_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_menu_separator_create(sub_timeAndDate_page);

    settings_timeAndDate_setNtpSwitch = create_switch(section, NULL, "Set from NTP", false);
    lv_obj_add_event_cb(settings_timeAndDate_setNtpSwitch, settings_timeAndDate_setNtpSwitch_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_menu_separator_create(sub_timeAndDate_page);

    section = lv_menu_section_create(sub_timeAndDate_page);
    settings_timeAndDate_syncTimeAndDateBtn = lv_button_create(section);
    lv_obj_set_width(settings_timeAndDate_syncTimeAndDateBtn, LV_PCT(100));
    lv_obj_t* syncTimeAndDateBtn_label = lv_label_create(settings_timeAndDate_syncTimeAndDateBtn);
    lv_obj_center(syncTimeAndDateBtn_label);
    lv_label_set_text(syncTimeAndDateBtn_label, "Sync Time & Date From NTP");
    lv_obj_add_state(settings_timeAndDate_syncTimeAndDateBtn, LV_STATE_DISABLED);
    lv_obj_add_event_cb(settings_timeAndDate_syncTimeAndDateBtn, settings_timeAndDate_syncTimeAndDateBtn_event_handler, LV_EVENT_CLICKED, NULL);

    lv_menu_separator_create(sub_timeAndDate_page);

    lv_obj_t* sub_update_page = lv_menu_page_create(menu, "Updates");
    lv_obj_set_style_pad_hor(sub_update_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), LV_PART_MAIN), 0);
    lv_menu_separator_create(sub_update_page);

    section = lv_menu_section_create(sub_update_page);
    settings_updates_checkForUpdatesBtn = lv_button_create(section);
    lv_obj_set_width(settings_updates_checkForUpdatesBtn, LV_PCT(100));
    lv_obj_t* checkForUpdatesBtn_label = lv_label_create(settings_updates_checkForUpdatesBtn);
    lv_obj_center(checkForUpdatesBtn_label);
    lv_label_set_text(checkForUpdatesBtn_label, LV_SYMBOL_DOWNLOAD " Check for Updates");
    lv_obj_add_state(settings_updates_checkForUpdatesBtn, LV_STATE_DISABLED); //enables when wifi is connected
    lv_obj_add_event_cb(settings_updates_checkForUpdatesBtn, settings_updates_checkForUpdatesBtn_event_handler, LV_EVENT_CLICKED, NULL);

    lv_menu_separator_create(sub_update_page);

    section = lv_menu_section_create(sub_update_page);
    settings_updates_updateStatusLabel = create_text(section, NULL, "Status...", LV_MENU_ITEM_BUILDER_VARIANT_1);

    lv_menu_separator_create(sub_update_page);

    section = lv_menu_section_create(sub_update_page);
    lv_obj_t* settings_updates_startUpdateBtn = lv_button_create(section);
    lv_obj_set_width(settings_updates_startUpdateBtn, LV_PCT(100));
    lv_obj_t* startUpdateBtn_label = lv_label_create(settings_updates_startUpdateBtn);
    lv_obj_center(startUpdateBtn_label);
    lv_label_set_text(startUpdateBtn_label, "Start Update");
    lv_obj_add_state(settings_updates_startUpdateBtn, LV_STATE_DISABLED);
    lv_obj_add_event_cb(settings_updates_startUpdateBtn, settings_updates_startUpdateBtn_event_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t* sub_software_info_page = lv_menu_page_create(menu, "Software Info");
    lv_obj_set_style_pad_hor(sub_software_info_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), LV_PART_MAIN), 0);
    section = lv_menu_section_create(sub_software_info_page);
    create_text(section, NULL, "Version 1.0", LV_MENU_ITEM_BUILDER_VARIANT_1);

    lv_obj_t* sub_about_page = lv_menu_page_create(menu, "About");
    lv_obj_set_style_pad_hor(sub_about_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), LV_PART_MAIN), 0);
    lv_menu_separator_create(sub_about_page);
    section = lv_menu_section_create(sub_about_page);
    cont = create_text(section, NULL, "Software information", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_software_info_page);


    /*Create a root page*/
    lv_obj_t* root_page = lv_menu_page_create(menu, "Settings");
    lv_obj_set_style_pad_hor(root_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), LV_PART_MAIN), 0);
    section = lv_menu_section_create(root_page);
    cont = create_text(section, LV_SYMBOL_WIFI, "Wi-Fi", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_wifi_page);
    cont = create_text(section, LV_SYMBOL_AUDIO, "Sound", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_sound_page);
    cont = create_text(section, LV_SYMBOL_IMAGE, "Display", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_display_page);
    cont = create_text(section, LV_SYMBOL_EDIT, "Time & Date", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_timeAndDate_page);
    cont = create_text(section, LV_SYMBOL_REFRESH, "Software Updates", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_update_page);

    create_text(root_page, NULL, "Others", LV_MENU_ITEM_BUILDER_VARIANT_1);
    section = lv_menu_section_create(root_page);
    cont = create_text(section, NULL, "About", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_about_page);

    lv_menu_set_page(menu, root_page);

    lv_obj_t* settingAppButton = lv_btn_create(appsScreen_cont);
    lv_obj_set_size(settingAppButton, 64, 64);
    lv_obj_add_event_cb(settingAppButton, settingsButton_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* settingAppButton_label = lv_label_create(settingAppButton);
    lv_label_set_text(settingAppButton_label, LV_SYMBOL_SETTINGS);
    lv_obj_set_style_text_font(settingAppButton_label, &lv_font_montserrat_42, 0);
    lv_obj_center(settingAppButton_label);
}

//apps should be coded in their own header files and included here

void setup_AllApps() {
    settings_app = lv_obj_create(NULL);

    setup_settings();
    setup_homeButtonApp(); //should always be last
}

void update_controls(bool fromControlPanel) {
    //if its from the control panel, update the controls in settings and vice versa
    if (fromControlPanel) {
        if (lv_obj_has_state(controlPanel_wifiButton, LV_STATE_CHECKED)) {
            lv_obj_add_state(settings_wifi_switch, LV_STATE_CHECKED);
        }
        else {
            lv_obj_remove_state(settings_wifi_switch, LV_STATE_CHECKED);
        }

        if (lv_obj_has_state(controlPanel_soundButton, LV_STATE_CHECKED)) {
            lv_obj_add_state(settings_sound_switch, LV_STATE_CHECKED);
        }
        else {
            lv_obj_remove_state(settings_sound_switch, LV_STATE_CHECKED);
        }

        lv_slider_set_value(settings_display_brightness_slider, lv_slider_get_value(controlPanel_brightnessSlider), LV_ANIM_OFF);
        lv_slider_set_value(settings_sound_volume_slider, lv_slider_get_value(controlPanel_volumeSlider), LV_ANIM_OFF);
    }
    else {
        if (lv_obj_has_state(settings_wifi_switch, LV_STATE_CHECKED)) {
            lv_obj_add_state(controlPanel_wifiButton, LV_STATE_CHECKED);
        }
        else {
            lv_obj_remove_state(controlPanel_wifiButton, LV_STATE_CHECKED);
        }

        if (lv_obj_has_state(settings_sound_switch, LV_STATE_CHECKED)) {
            lv_obj_add_state(controlPanel_soundButton, LV_STATE_CHECKED);
        }
        else {
            lv_obj_remove_state(controlPanel_soundButton, LV_STATE_CHECKED);
        }
        lv_slider_set_value(controlPanel_brightnessSlider, lv_slider_get_value(settings_display_brightness_slider), LV_ANIM_OFF);
        lv_slider_set_value(controlPanel_volumeSlider, lv_slider_get_value(settings_sound_volume_slider), LV_ANIM_OFF);
    }
}

static void controlPanel_wifiButton_event_handler(lv_event_t* e)
{
    update_controls(true);
    if (lv_obj_has_state(controlPanel_wifiButton, LV_STATE_CHECKED)) {
        //enable wifi
        UI_set_wifi_enabled(true);
    }
    else {
        //disable wifi
        UI_set_wifi_enabled(false);
    }
}

static void controlPanel_soundButton_event_handler(lv_event_t* e)
{
    update_controls(true);
    if (lv_obj_has_state(controlPanel_soundButton, LV_STATE_CHECKED)) {
        //enable sound
        UI_set_sound_enabled(true);
    }
    else {
        //disable sound
        UI_set_sound_enabled(false);
    }
}

static void controlPanel_brightnessSlider_event_handler(lv_event_t* e)
{
    update_controls(true);
    UI_change_brightness(lv_slider_get_value(controlPanel_brightnessSlider));
}
static void controlPanel_volumeSlider_event_handler(lv_event_t* e)
{
    update_controls(true);
    UI_change_volume(lv_slider_get_value(controlPanel_volumeSlider));
}

static void settings_wifi_switch_event_handler(lv_event_t* e)
{
    update_controls(false);
    if (lv_obj_has_state(settings_wifi_switch, LV_STATE_CHECKED)) {
        //enable wifi
        UI_set_wifi_enabled(true);
    }
    else {
        //disable wifi
        UI_set_wifi_enabled(false);
    }
}

static void settings_wifi_scan_btn_event_handler(lv_event_t* e)
{
    //scan for wifi networks and populate the list
    UI_start_wifi_scan(); //this function should populate the list when done
}

static void settings_sound_switch_event_handler(lv_event_t* e)
{
    update_controls(false);
    if (lv_obj_has_state(settings_sound_switch, LV_STATE_CHECKED)) {
        //enable sound
        UI_set_sound_enabled(true);
    }
    else {
        //disable sound
        UI_set_sound_enabled(false);
    }
}

static void settings_display_brightness_slider_event_handler(lv_event_t* e)
{
    update_controls(false);
    UI_change_brightness(lv_slider_get_value(settings_display_brightness_slider));
}

static void settings_display_timeout_roller_event_handler(lv_event_t* e) {
    //set screen timeout based on selected option
    lv_obj_t* obj = (lv_obj_t*)lv_event_get_current_target(e);
    int selectedIndex = lv_roller_get_selected(obj);
    switch (selectedIndex) {
    case 0:
        UI_set_display_timeout(5); //5s
        break;
    case 1:
        UI_set_display_timeout(10); //10s
        break;
    case 2:
        UI_set_display_timeout(15); //15s
        break;
    case 3:
        UI_set_display_timeout(30); //30s
        break;
    case 4:
        UI_set_display_timeout(60); //1m
        break;
    case 5:
        UI_set_display_timeout(300); //5m
        break;
    case 6:
        UI_set_display_timeout(0); //never
        break;
    }
}

static void settings_sound_volume_slider_event_handler(lv_event_t* e)
{
    update_controls(false);
    UI_change_volume(lv_slider_get_value(settings_sound_volume_slider));
}
static void settings_timeAndDate_calendar_event_handler(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = (lv_obj_t*)lv_event_get_current_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        lv_calendar_date_t date;
        if (lv_calendar_get_pressed_date(obj, &date)) {
            lv_calendar_set_highlighted_dates(obj, &date, 1);
            settings_timeAndDate_selectedDate = date;
        }
    }
}

static void settings_timeAndDate_setBtn_event_handler(lv_event_t* e)
{
    //get time from rollers
    int hour = 0;
    int min = 0;
    bool is24hr = lv_obj_has_state(settings_timeAndDate_24hrSwitch, LV_STATE_CHECKED);
    if (is24hr) {
        hour = lv_roller_get_selected(settings_timeAndDate_hrRoller);
    }
    else {
        hour = lv_roller_get_selected(settings_timeAndDate_hrRoller) + 1; //rollers are 0 indexed
        int selectedIndex = lv_roller_get_selected(settings_timeAndDate_amPmRoller);
        if (selectedIndex == 0 && hour != 12) {
            hour += 12;
        }
        else if (selectedIndex == 1 && hour == 12) {
            hour = 0;
        }
    }
    min = lv_roller_get_selected(settings_timeAndDate_minRoller);
    //set time
    UI_set_time(hour, min);

    //get date from calendar
    lv_calendar_date_t date = settings_timeAndDate_selectedDate;
    if (date.year == 0) {
        return;
    }
    UI_set_date(date.year, date.month, date.day);

    //clear selected date and set today's date
    settings_timeAndDate_selectedDate = {};
    lv_calendar_set_today_date(settings_timeAndDate_calendar, date.year, date.month, date.day);
}

static void settings_timeAndDate_24hrSwitch_event_handler(lv_event_t* e) {
    //update roller options and disable am/pm roller if 24hr is selected
    lv_obj_t* obj = (lv_obj_t*)lv_event_get_current_target(e);
    if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
        lv_roller_set_options(settings_timeAndDate_hrRoller, settings_timeAndDate_hrRoller_24hr, LV_ROLLER_MODE_INFINITE);
        lv_obj_add_flag(settings_timeAndDate_amPmRoller, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_width(settings_timeAndDate_hrRoller, lv_pct(50));
        lv_obj_set_width(settings_timeAndDate_minRoller, lv_pct(50));
    }
    else {
        lv_roller_set_options(settings_timeAndDate_hrRoller, settings_timeAndDate_hrRoller_12hr, LV_ROLLER_MODE_INFINITE);
        lv_obj_clear_flag(settings_timeAndDate_amPmRoller, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_width(settings_timeAndDate_hrRoller, lv_pct(33));
        lv_obj_set_width(settings_timeAndDate_minRoller, lv_pct(33));
    }
    //update UI as well
    is24HourFormat = lv_obj_has_state(obj, LV_STATE_CHECKED);
}

static void settings_timeAndDate_setNtpSwitch_event_handler(lv_event_t* e) {
    //disable all time and date controls if enabled
    lv_obj_t* obj = (lv_obj_t*)lv_event_get_current_target(e);
    if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
        lv_obj_add_state(settings_timeAndDate_setBtn, LV_STATE_DISABLED);
        lv_obj_clear_state(settings_timeAndDate_syncTimeAndDateBtn, LV_STATE_DISABLED);
    }
    else {
        lv_obj_clear_state(settings_timeAndDate_setBtn, LV_STATE_DISABLED);
        lv_obj_add_state(settings_timeAndDate_syncTimeAndDateBtn, LV_STATE_DISABLED);
    }
}

static void settings_timeAndDate_syncTimeAndDateBtn_event_handler(lv_event_t* e) {
    UI_sync_time_and_date_from_ntp();
}

static void settings_updates_checkForUpdatesBtn_event_handler(lv_event_t* e) {
    UI_check_for_software_updates();
}
static void settings_updates_startUpdateBtn_event_handler(lv_event_t* e) {
    UI_start_software_update();
}

void setup_screens() {
    setupTopBarTimer(); //prolly the best place to put it
    homeScreen = lv_obj_create(NULL);
    appsScreen = lv_obj_create(NULL);

    setup_homeScreen();
    setup_appsScreen();

    //top bar and control panel
    setup_topBar(lv_layer_top());
    setup_controlPanel(lv_layer_top());
    lv_obj_set_y(controlPanel, -lv_obj_get_height(controlPanel)); // hide it initially

    setup_AllApps();
}

lv_obj_t* background;
lv_anim_t animation_template; //for scrolling the toast text
lv_style_t label_style;

void show_toast(const char* text, uint32_t duration) {
    if (lv_obj_is_valid(background)) {
        lv_obj_delete(background);
    }

    static lv_style_t style;
    lv_style_init(&style);

    lv_style_set_bg_opa(&style, LV_OPA_70);

    background = lv_obj_create(lv_layer_top());
    lv_obj_null_on_delete(&background);
    lv_obj_add_style(background, &style, 0);
    lv_obj_set_size(background, LV_PCT(90), LV_SIZE_CONTENT);
    lv_obj_clear_flag(background, LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    lv_obj_move_to_index(background, lv_obj_get_index(background) - 1);
    lv_obj_update_layout(background);
    lv_obj_align(background, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_event_cb(background, [](lv_event_t* e) {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, background);
        lv_anim_set_values(&a, -20, 20 + lv_obj_get_height(background));
        lv_anim_set_duration(&a, 200);
        lv_anim_set_exec_cb(&a, anim_y_cb);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
        lv_anim_set_completed_cb(&a, [](lv_anim_t* a) {
            lv_obj_t* obj = (lv_obj_t*)a->var;
            if (obj)
                lv_obj_del(obj);
            });
        lv_anim_start(&a);
        }, LV_EVENT_CLICKED, background);



    lv_anim_init(&animation_template);
    lv_anim_set_duration(&animation_template, 500);
    lv_anim_set_delay(&animation_template, 500);           /*Wait 1 second to start the first scroll*/
    lv_anim_set_repeat_delay(&animation_template, 1000);
    lv_anim_set_completed_cb(&animation_template, [](lv_anim_t* t) {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, background);
        lv_anim_set_values(&a, -20, 20 + lv_obj_get_height(background));
        lv_anim_set_duration(&a, 200);
        lv_anim_set_exec_cb(&a, anim_y_cb);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
        lv_anim_set_completed_cb(&a, [](lv_anim_t* a) {
            lv_obj_t* obj = (lv_obj_t*)a->var;
            if (obj)
                lv_obj_del(obj);
            });
        lv_anim_start(&a);
        });

    /*Initialize the label style with the animation template*/
    lv_style_init(&label_style);
    lv_style_set_anim(&label_style, &animation_template);

    lv_obj_t* toast = lv_label_create(background);
    lv_obj_set_width(toast, LV_PCT(100));
    lv_label_set_long_mode(toast, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    lv_label_set_text(toast, text);
    lv_obj_set_style_text_font(toast, &lv_font_montserrat_14, 0);
    lv_obj_add_style(toast, &label_style, LV_STATE_DEFAULT);
    lv_obj_center(toast);

    lv_obj_set_y(background, 20 + lv_obj_get_height(background));
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, background);
    lv_anim_set_values(&a, 20 + lv_obj_get_height(background), -20);
    lv_anim_set_duration(&a, 300);
    lv_anim_set_exec_cb(&a, anim_y_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);
}

void update_homeScreen(DateTime now)
{
    // update time and date
    if (is24HourFormat)
    {
        lv_label_set_text_fmt(homeScreen_timeText, "%02i:%02i", now.hour(), now.minute());
    }
    else
    {
        if (now.hour() > 12)
        {
            lv_label_set_text_fmt(homeScreen_timeText, "%i:%02i PM", now.hour() - 12, now.minute());
        }
        else
        {
            if (now.hour() == 0)
            {
                lv_label_set_text_fmt(homeScreen_timeText, "%i:%02i AM", 12, now.minute());
            }
            else
            {
                lv_label_set_text_fmt(homeScreen_timeText, "%i:%02i AM", now.hour(), now.minute());
            }
        }
    }
    lv_label_set_text_fmt(homeScreen_dateText, "%i-%i-%i", now.month(), now.day(), now.year());
}
