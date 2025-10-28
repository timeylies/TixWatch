#pragma once
//#include <lvgl.h>
// lvgl screens

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

#include <cstdio> //for logs

void setup_topBar(lv_obj_t* screen) {
    topBar = lv_obj_create(screen);
    lv_obj_align(topBar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_size(topBar, TFT_WIDTH - 5, 30);
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
    }
    else {
        int lvl = batPercent;
        if (lvl > 85 && lvl < 100) {
            lv_label_set_text(topBar_batteryIcon, LV_SYMBOL_BATTERY_FULL);
        }
        if (lvl > 75 && lvl < 85) {
            lv_label_set_text(topBar_batteryIcon, LV_SYMBOL_BATTERY_3);
        }
        if (lvl > 50 && lvl < 75) {
            lv_label_set_text(topBar_batteryIcon, LV_SYMBOL_BATTERY_2);
        }
        if (lvl > 25 && lvl < 50) {
            lv_label_set_text(topBar_batteryIcon, LV_SYMBOL_BATTERY_1);
        }
        if (lvl >= 0 && lvl < 25) {
            lv_label_set_text(topBar_batteryIcon, LV_SYMBOL_BATTERY_EMPTY);
        }
    }
    lv_label_set_text_fmt(topBar_batteryText, "%i%%", batPercent);

}

lv_obj_t* controlPanel;
lv_obj_t* controlPanel_wifiButton;
lv_obj_t* controlPanel_soundButton;
lv_obj_t* controlPanel_brightnessSlider;
lv_obj_t* controlPanel_volumeSlider;
lv_obj_t* controlPanel_closeButton;

void setup_controlPanel(lv_obj_t* screen) {
    controlPanel = lv_obj_create(screen);
    lv_obj_align(controlPanel, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_size(controlPanel, TFT_WIDTH - 5, 150);
    lv_obj_remove_flag(controlPanel, LV_OBJ_FLAG_SCROLLABLE);

    controlPanel_wifiButton = lv_btn_create(controlPanel);
    lv_obj_set_size(controlPanel_wifiButton, LV_PCT(48), LV_SIZE_CONTENT);
    lv_obj_align(controlPanel_wifiButton, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_add_flag(controlPanel_wifiButton, LV_OBJ_FLAG_CHECKABLE);

    lv_obj_t* controlPanel_wifiButtonIcon = lv_label_create(controlPanel_wifiButton);
    lv_label_set_text(controlPanel_wifiButtonIcon, LV_SYMBOL_WIFI " Wi-Fi");
    lv_obj_align(controlPanel_wifiButtonIcon, LV_ALIGN_CENTER, 0, 0);

    controlPanel_soundButton = lv_btn_create(controlPanel);
    lv_obj_set_size(controlPanel_soundButton, LV_PCT(48), LV_SIZE_CONTENT);
    lv_obj_align(controlPanel_soundButton, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_flag(controlPanel_soundButton, LV_OBJ_FLAG_CHECKABLE);

    lv_obj_t* controlPanel_soundButtonIcon = lv_label_create(controlPanel_soundButton);
    lv_label_set_text(controlPanel_soundButtonIcon, LV_SYMBOL_VOLUME_MAX " Sound");
    lv_obj_align(controlPanel_soundButtonIcon, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* controlPanel_brightnessSliderIcon = lv_label_create(controlPanel);
    lv_label_set_text(controlPanel_brightnessSliderIcon, LV_SYMBOL_IMAGE);
    lv_obj_set_style_text_font(controlPanel_brightnessSliderIcon, &lv_font_montserrat_18, 0);
    lv_obj_align_to(controlPanel_brightnessSliderIcon, controlPanel_wifiButton, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
    lv_obj_set_x(controlPanel_brightnessSliderIcon, LV_PCT(1));

    controlPanel_brightnessSlider = lv_slider_create(controlPanel);
    lv_obj_align_to(controlPanel_brightnessSlider, controlPanel_brightnessSliderIcon, LV_ALIGN_OUT_RIGHT_MID, 15, 1);
    lv_obj_set_size(controlPanel_brightnessSlider, LV_PCT(80), 10);

    lv_obj_t* controlPanel_volumeSliderIcon = lv_label_create(controlPanel);
    lv_label_set_text(controlPanel_volumeSliderIcon, LV_SYMBOL_VOLUME_MAX);
    lv_obj_align_to(controlPanel_volumeSliderIcon, controlPanel_brightnessSliderIcon, LV_ALIGN_OUT_BOTTOM_MID, 0, 13);

    controlPanel_volumeSlider = lv_slider_create(controlPanel);
    lv_obj_align_to(controlPanel_volumeSlider, controlPanel_volumeSliderIcon, LV_ALIGN_OUT_RIGHT_MID, 16, 1);
    lv_obj_set_size(controlPanel_volumeSlider, LV_PCT(80), 10);

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
        if (topBarControlPanelState != controlPanelShowing || topBarControlPanelState != topBarHidden) {
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
        lv_obj_set_parent(topBar, appsScreen);
        lv_obj_set_parent(controlPanel, appsScreen);
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

    lv_obj_set_parent(topBar, homeScreen);
    lv_obj_set_parent(controlPanel, homeScreen);
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

lv_obj_t* settings_app;


void settingsButton_event_cb(lv_event_t* event) {
    lv_obj_set_parent(topBar, settings_app);
    lv_obj_set_parent(controlPanel, settings_app);
    lv_screen_load_anim(settings_app, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
}

void setup_settings() {



    lv_obj_t* obj = lv_btn_create(appsScreen_cont);
    lv_obj_set_size(obj, 64, 64);
    lv_obj_add_event_cb(obj, settingsButton_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* label = lv_label_create(obj);
    lv_label_set_text(label, LV_SYMBOL_SETTINGS);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_42, 0);
    lv_obj_center(label);
}

void setup_AllApps() {
    settings_app = lv_obj_create(NULL);

    setup_settings();
    setup_homeButtonApp(); //should always be last
}

void setup_screens() {
    setupTopBarTimer(); //prolly the best place to put it
    homeScreen = lv_obj_create(NULL);
    appsScreen = lv_obj_create(NULL);

    setup_homeScreen();
    setup_appsScreen();

    //top bar and control panel
    setup_topBar(homeScreen);
    setup_controlPanel(homeScreen);
    lv_obj_set_y(controlPanel, -lv_obj_get_height(controlPanel)); // hide it initially

    setup_AllApps();
}


void update_homeScreen(DateTime now)
{
  // update time and date
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
  lv_label_set_text_fmt(homeScreen_dateText, "%i-%i-%i", now.month(), now.day(), now.year());
  // log_i("Updated Home Screen");
}
