#include "config.h"
#include "hardware.h"

// to shut the errors up
#define TFT_WIDTH 240
#define TFT_HEIGHT 240

bool irq = false;

// WARNING: This function is called from a separate FreeRTOS task (thread)!
void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);

  switch (event) {
    case ARDUINO_EVENT_WIFI_READY:               Serial.println("WiFi interface ready"); show_toast("WiFi interface ready", 2000); break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:           Serial.println("Completed scan for access points"); /*stop loading animation*/ break;
    case ARDUINO_EVENT_WIFI_STA_START:           Serial.println("WiFi client started"); break;
    case ARDUINO_EVENT_WIFI_STA_STOP:            Serial.println("WiFi clients stopped"); break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:       Serial.println("Connected to access point"); show_toast("WiFi connected", 2000); break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:    Serial.println("Disconnected from WiFi access point"); show_toast("WiFi disconnected", 2000); break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE: Serial.println("Authentication mode of access point has changed"); break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("Obtained IP address: ");
      Serial.println(WiFi.localIP());
      break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:        Serial.println("Lost IP address and IP address is reset to 0"); show_toast("Lost IP address and IP address is reset to 0", 2000); break;
    case ARDUINO_EVENT_WPS_ER_SUCCESS:          Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_FAILED:           Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:          Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_PIN:              Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode"); break;
    default:                                    break;
  }
}

// WARNING: This function is called from a separate FreeRTOS task (thread)!
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
  
}

void UI_set_wifi_enabled(bool enabled) {
  if(enabled){
    WiFi.begin();
  } else {
    WiFi.disconnect(true);
  }
}

void UI_change_brightness(int brightness) {
  display_change_brightness(brightness);
}

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

  // button stuff
  attachInterrupt(AXP202_INTERUPT, []
                  { irq = true; }, FALLING);

                  
  // start wifi stuff
  WiFi.onEvent(WiFiEvent);
  WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFiEventId_t eventID = WiFi.onEvent(
    [](WiFiEvent_t event, WiFiEventInfo_t info) {
      Serial.print("WiFi lost connection. Reason: ");
      Serial.println(info.wifi_sta_disconnected.reason);
    },
    WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED
  );
}



void loop()
{
  lv_timer_handler();
  process_toast_queue();
  update_homeScreen(rtc.now());
  update_topBarBatteryStatus(power->isVBUSPlug(), power->getBattPercentage());

  // Use side button as back button
  if (irq)
  {
    irq = false;
    power->readIRQ();
    if (power->isPEKShortPressIRQ())
    {
      if (lv_screen_active() == homeScreen)
      {
        // TODO: Put watch into sleep when in homescreen
        show_toast("Side Button Pressed and in home screen but sleep not implemented", 10000);
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