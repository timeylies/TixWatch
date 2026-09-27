#include "config.h"
#include "hardware.h"

// to shut the errors up
#define TFT_WIDTH 240
#define TFT_HEIGHT 240

volatile bool wifi_scan_complete = false;
volatile bool wifi_status_changed = false;

// WARNING: This function is called from a separate FreeRTOS task (thread)!
void WiFiEvent(WiFiEvent_t event)
{
  Serial.printf("[WiFi-event] event: %d\n", event);

  switch (event)
  {
  case ARDUINO_EVENT_WIFI_READY:
    Serial.println("WiFi interface ready");
    show_toast("WiFi interface ready", 2000);
    break;
  case ARDUINO_EVENT_WIFI_SCAN_DONE:
    Serial.println("Completed scan for access points");
    wifi_scan_complete = true;
    break;
  case ARDUINO_EVENT_WIFI_STA_START:
    Serial.println("WiFi client started");
    break;
  case ARDUINO_EVENT_WIFI_STA_STOP:
    Serial.println("WiFi clients stopped");
    break;
  case ARDUINO_EVENT_WIFI_STA_CONNECTED:
    Serial.println("Connected to access point");
    show_toast("WiFi connected, waiting for IP address...", 8000);
    break;
  case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
    Serial.println("Disconnected from WiFi access point"); /*show_toast("WiFi disconnected", 2000);*/
    wifi_status_changed = true;
    break;
  case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
    Serial.println("Authentication mode of access point has changed");
    break;
  case ARDUINO_EVENT_WIFI_STA_GOT_IP:
    Serial.print("Obtained IP address: ");
    Serial.println(WiFi.localIP());
    wifi_status_changed = true;
    break;
  case ARDUINO_EVENT_WIFI_STA_LOST_IP:
    Serial.println("Lost IP address and IP address is reset to 0");
    show_toast("Lost IP address and IP address is reset to 0", 2000);
    break;
  case ARDUINO_EVENT_WPS_ER_SUCCESS:
    Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
    break;
  case ARDUINO_EVENT_WPS_ER_FAILED:
    Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
    break;
  case ARDUINO_EVENT_WPS_ER_TIMEOUT:
    Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
    break;
  case ARDUINO_EVENT_WPS_ER_PIN:
    Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
    break;
  default:
    break;
  }
}

// WARNING: This function is called from a separate FreeRTOS task (thread)!
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
  show_toast("Got IP Address!", 3000);
}

void UI_set_wifi_enabled(bool enabled)
{
  if (enabled)
  {
    WiFi.begin();
    WiFi.setTxPower(WIFI_POWER_19_5dBm);
  }
  else
  {
    WiFi.disconnect(true);
  }
  prefs.begin("settings", false);
  prefs.putBool("wifi_on", enabled);
  prefs.end();
}

void UI_set_sound_enabled(bool enabled)
{
  prefs.begin("settings", false);
  prefs.putBool("sound_on", enabled);
  prefs.end();

  set_audio(enabled);
}

void UI_change_volume(int volume)
{
  prefs.begin("settings", false);
  prefs.putInt("volume", volume);
  prefs.end();
}

void UI_set_display_timeout(int seconds)
{
  screenTimeoutSeconds = seconds;
  lastActivityMillis = millis();
  prefs.begin("settings", false);
  prefs.putInt("timeout", seconds);
  prefs.end();
}

void UI_start_wifi_scan()
{
  // this automatically turns wifi on

  prefs.begin("settings", false);
  prefs.putBool("wifi_on", true);
  prefs.end();

  Set_UI_wifi(true);

  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
  WiFi.scanNetworks(true, true); // async, include hidden SSIDs — fires ARDUINO_EVENT_WIFI_SCAN_DONE when done
  lv_label_set_text(settings_wifi_scan_btn_text, LV_SYMBOL_REFRESH " Scanning...");
}

void UI_set_time(int hour, int minute)
{
  DateTime now = rtc.now();
  rtc.adjust(DateTime(now.year(), now.month(), now.day(), hour, minute, 0));
  show_toast("Time set!", 2000);
}

void UI_set_date(int year, int month, int day)
{
  DateTime now = rtc.now();
  rtc.adjust(DateTime(year, month, day, now.hour(), now.minute(), now.second()));
  show_toast("Date set!", 2000);
}

void UI_sync_time_and_date_from_ntp()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    show_toast("Connect to WiFi first / wait for IP Address!", 8000);
    return;
  }
  show_toast("Syncing time from NTP...", 4000);
  xTaskCreate([](void *)
              {
    configTzTime(timezone, "pool.ntp.org", "time.nist.gov");
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 10000)) {
      rtc.adjust(DateTime(
        timeinfo.tm_year + 1900,
        timeinfo.tm_mon + 1,
        timeinfo.tm_mday,
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec
      ));
      show_toast("Time synced from NTP!", 3000);
    } else {
      show_toast("NTP sync failed!", 3000);
    }
    vTaskDelete(NULL); }, "ntp_sync", 4096, NULL, 1, NULL);
}

void UI_change_brightness(int brightness)
{
  display_change_brightness(brightness);
  prefs.begin("settings", false);
  prefs.putInt("brightness", brightness);
  prefs.end();
}

void settings_load()
{
  prefs.begin("settings", true); // read-only

  // brightness
  int brightness = prefs.getInt("brightness", 100);
  Set_UI_brightness(brightness);
  display_change_brightness(brightness);

  // sound
  bool soundOn = prefs.getBool("sound_on", false);
  if (soundOn)
  {
    Set_UI_sound(true);
    set_audio(true);
  }

  // volume
  int volume = prefs.getInt("volume", 100);
  Set_UI_volume(volume);

  // wifi
  bool wifiOn = prefs.getBool("wifi_on", false);
  if (wifiOn)
  {
    Set_UI_wifi(true);
    WiFi.begin();
    WiFi.setTxPower(WIFI_POWER_19_5dBm);
  }

  // screen timeout — map seconds back to roller index
  int timeout = prefs.getInt("timeout", 0);
  screenTimeoutSeconds = timeout;
  Set_UI_display_timeout(timeout);

  // 24-hour format
  bool fmt24 = prefs.getBool("fmt_24hr", false);
  if (fmt24)
  {
    is24HourFormat = true;
    Set_UI_24hr(true);
  }

  // use NTP
  bool useNtp = prefs.getBool("use_ntp", false);
  if (useNtp)
  {
    Set_UI_ntp(true);
  }

  prefs.end();
}

void setup()
{
  Serial.begin(115200);
  pinMode(VIBRATOR, OUTPUT);
  digitalWrite(VIBRATOR, 1); //as a quick way to show that the watch is on

  // run the hardware setup first here
  hardware_init();
  digitalWrite(VIBRATOR, 0);
  lvgl_init();

  setup_screens();
  settings_load();
  lv_screen_load(homeScreen);

  lv_obj_add_event_cb(topBar, topBar_swipe_cb, LV_EVENT_GESTURE, NULL);
  lv_obj_add_event_cb(controlPanel_closeButton, controlPanel_close_cb, LV_EVENT_CLICKED, NULL);

  // start wifi stuff
  WiFi.onEvent(WiFiEvent);
  WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFiEventId_t eventID = WiFi.onEvent(
      [](WiFiEvent_t event, WiFiEventInfo_t info)
      {
        Serial.print("WiFi lost connection. Reason: ");
        Serial.println(info.wifi_sta_disconnected.reason);
      },
      WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
}

void loop()
{
  lv_timer_handler();
  process_toast_queue();
  if (wifi_scan_complete)
  {
    wifi_scan_complete = false;
    UI_populate_wifi_list();
  }
  if (wifi_status_changed)
  {
    wifi_status_changed = false;
    UI_populate_wifi_list();
    update_topBar_icons();
  }
  DateTime now = rtc.now();
  update_homeScreen(now);
  update_topBar_time(now);
  update_topBarBatteryStatus(power->isVBUSPlug(), power->getBattPercentage());

  // Screen timeout
  if (screenTimeoutSeconds > 0)
  {
    uint32_t elapsed = millis() - lastActivityMillis;
    uint32_t timeoutMs = (uint32_t)screenTimeoutSeconds * 1000;
    if (elapsed >= timeoutMs)
    {
      enter_light_sleep();
    }
    else if (timeoutMs > 3000 && elapsed >= timeoutMs - 3000 && !screenDimmed)
    {
      screenDimmed = true;
      int startBrightness = lv_slider_get_value(controlPanel_brightnessSlider);
      for (int b = startBrightness; b > 10; b -= 2)
      {
        display_change_brightness(b);
        delay(15);
      }
      display_change_brightness(10);
    }
  }

  // Use side button as back button
  if (axp_irq)
  {
    axp_irq = false;
    lastActivityMillis = millis(); // side button press counts as activity
    if (screenDimmed)
    {
      screenDimmed = false;
      display_change_brightness(lv_slider_get_value(controlPanel_brightnessSlider));
    }
    power->readIRQ();
    if(power->isChargingIRQ()){
      play_charging();
    }
    if (power->isPEKShortPressIRQ())
    {
      if (lv_screen_active() == homeScreen)
      {
        enter_light_sleep();
      }
      if (lv_screen_active() == settings_app)
      {
        // load home screen
        lv_obj_set_parent(topBar, appsScreen);
        lv_obj_set_parent(controlPanel, appsScreen);
        lv_screen_load_anim(appsScreen, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
      }
      if (lv_screen_active() == appsScreen)
      {
        // load home screen
        lv_obj_set_parent(topBar, homeScreen);
        lv_obj_set_parent(controlPanel, homeScreen);
        lv_screen_load_anim(homeScreen, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
      }
    }
    power->clearIRQ();
  }
}