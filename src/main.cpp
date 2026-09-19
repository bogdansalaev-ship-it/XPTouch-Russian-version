#include <driver/i2s.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include "xtouch/globals.h"
#include "xtouch/debug.h"
#include "xtouch/paths.h"
#include "xtouch/eeprom.h"
#include "xtouch/types.h"
#include "xtouch/bblp.h"
#include "xtouch/globals.h"
#include "xtouch/filesystem.h"
#include "ui/ui.h"
#include "xtouch/sdcard.h"
#include "xtouch/hms.h"

#if defined(__XTOUCH_SCREEN_28__)
#include "devices/2.8/screen.h"
#endif

#if defined(__XTOUCH_SCREEN_50__)
#include "devices/5.0/screen.h"
#endif

#include "xtouch/cloud.hpp"
#include "xtouch/neopixel.h"
#include "xtouch/led.h"
#include "xtouch/settings.h"
#include "xtouch/net.h"
#include "xtouch/firmware.h"
#include "xtouch/mqtt.h"
#if defined(__XTOUCH_SCREEN_50__)
#include "xtouch/thumbnail.h"
#include "xtouch/history.h"
#include "xtouch/lv_fs_arduino_sd.h"
#include "xtouch/lcd_json.h"
#endif
#include "xtouch/sensors/chamber.h"
#include "xtouch/events.h"
#include "xtouch/connection.h"
#include "xtouch/coldboot.h"
#include "xtouch/webserver.h"
#include "xtouch/globals.h"
#include "xtouch/filaments.h"
#include "xtouch/ams_edit_temp.h"
#include "xtouch/filaments_rev.h"
#include "ui/ui_events.h"

// #if defined(__XTOUCH_SCREEN_28__)
// #include "xtouch/m5Stack.h"
// #endif

void xtouch_intro_show(void)
{
  ui_introScreen_screen_init();
  lv_disp_load_scr(introScreen);
  lv_timer_handler();
}

// Flag set by UI to request OTA update flow. Defined here so other modules can
// reference it without introducing a separate OTA module after OTA removal.
bool xtouch_ota_update_flag = false;

void setup()
{
  Serial.begin(115200);
#if defined(XTOUCH_DEBUG) || XTOUCH_USE_SERIAL == true || XTOUCH_DEBUG_ERROR == true || XTOUCH_DEBUG_DEBUG == true || XTOUCH_DEBUG_INFO == true|| XTOUCH_DEBUG_VERBOSE == true
  Serial.begin(115200);
  ConsoleDebug.println("Serial started");
#endif

  xtouch_eeprom_setup();
#if defined(__XTOUCH_SCREEN_50__)
  xtouch_eeprom_rgb_pclk_heal_invalid_storage();
#endif
  xtouch_globals_init();
  xtouch_screen_setup();

#ifdef __XTOUCH_SCREEN_50__
  lv_font_small_set(&lv_font_montserrat_28);
  lv_font_middle_set(&lv_font_montserrat_32);
  lv_font_big_set(&lv_font_montserrat_48);
  lv_icon_font_small_set(&ui_font_xlcd48);
#else
  lgfx::boards::board_t board = tft.getBoard();

  if (board == lgfx::boards::board_t::board_Sunton_ESP32_3248S035C ||
      board == lgfx::boards::board_t::board_Sunton_ESP32_3248S035R)
  {
    ConsoleDebug.println("Found Sunton_ESP32_3248S035C or Sunton_ESP32_3248S035R");
    lv_font_small_set(&lv_font_montserrat_24);
    lv_font_middle_set(&lv_font_montserrat_28);
    lv_font_big_set(&lv_font_montserrat_32);
  }
#endif

  extern const lv_font_t rus;
  static lv_font_t proxy_small, proxy_middle, proxy_big;
#ifdef lv_font_small
#undef lv_font_small
#undef lv_font_middle
#undef lv_font_big
#endif
  const lv_font_t *src_small = lv_font_small();
  const lv_font_t *src_middle = lv_font_middle();
  const lv_font_t *src_big = lv_font_big();
  proxy_small.get_glyph_dsc = src_small->get_glyph_dsc;
  proxy_small.get_glyph_bitmap = src_small->get_glyph_bitmap;
  proxy_small.line_height = src_small->line_height;
  proxy_small.base_line = src_small->base_line;
  proxy_small.subpx = src_small->subpx;
  proxy_small.underline_position = src_small->underline_position;
  proxy_small.underline_thickness = src_small->underline_thickness;
  proxy_small.dsc = src_small->dsc;
  proxy_small.fallback = &rus;
  proxy_middle.get_glyph_dsc = src_middle->get_glyph_dsc;
  proxy_middle.get_glyph_bitmap = src_middle->get_glyph_bitmap;
  proxy_middle.line_height = src_middle->line_height;
  proxy_middle.base_line = src_middle->base_line;
  proxy_middle.subpx = src_middle->subpx;
  proxy_middle.underline_position = src_middle->underline_position;
  proxy_middle.underline_thickness = src_middle->underline_thickness;
  proxy_middle.dsc = src_middle->dsc;
  proxy_middle.fallback = &rus;
  proxy_big.get_glyph_dsc = src_big->get_glyph_dsc;
  proxy_big.get_glyph_bitmap = src_big->get_glyph_bitmap;
  proxy_big.line_height = src_big->line_height;
  proxy_big.base_line = src_big->base_line;
  proxy_big.subpx = src_big->subpx;
  proxy_big.underline_position = src_big->underline_position;
  proxy_big.underline_thickness = src_big->underline_thickness;
  proxy_big.dsc = src_big->dsc;
  proxy_big.fallback = &rus;
  lv_font_small_set(&proxy_small);
  lv_font_middle_set(&proxy_middle);
  lv_font_big_set(&proxy_big);

  initTopLayer();

  xtouch_intro_show();
  int8_t sd_cs_pin = -1;  
  // 2.8": M5Stack のみ 4、それ以外は -1(デフォルト)
#if defined(__XTOUCH_SCREEN_50__)
  sd_cs_pin = 10;
#elif defined(__XTOUCH_SCREEN_28__)
  if (tft.getBoard() == (lgfx::boards::board_t)2) {
      sd_cs_pin = 4;
  }else{
      sd_cs_pin = 5;
  }
#endif
  while (!xtouch_sdcard_setup(sd_cs_pin))
    ;

#if defined(__XTOUCH_SCREEN_50__)
  xtouch_lcd_json_apply_from_sd_and_reboot();
  lv_fs_arduino_sd_init();  /* LVGL FS ドライバ: Arduino SD を 'S:' として登録 */
  xtouch_thumbnail_subscribe_events();
  xtouch_history_subscribe_events();
#endif

  xtouch_coldboot_check();

  xtouch_settings_loadSettings();

  xtouch_firmware_checkFirmwareUpdate();

  xtouch_touch_setup();

  while (!xtouch_wifi_setup())
    ;

  ArduinoOTA.setHostname("xtouch");
  ArduinoOTA.onStart([]() {
    lv_label_set_text(introScreenCaption, " OTA: начало обновления");
    lv_timer_handler();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    char message[32];
    snprintf(message, sizeof(message), " OTA: %u%%", (progress * 100U) / total);
    lv_label_set_text(introScreenCaption, message);
    lv_timer_handler();
  });
  ArduinoOTA.onEnd([]() {
    lv_label_set_text(introScreenCaption, " OTA: готово");
    lv_timer_handler();
  });
  ArduinoOTA.onError([](ota_error_t error) {
    char message[32];
    snprintf(message, sizeof(message), " OTA: ошибка %u", (unsigned)error);
    lv_label_set_text(introScreenCaption, message);
    lv_timer_handler();
  });
  ArduinoOTA.begin();

  xtouch_screen_setupScreenTimer();
  xtouch_screen_setupLEDOffTimer();
  xtouch_setupGlobalEvents();
    if(xtouch_filesystem_exist(SD, xtouch_paths_provisioning)){
        ConsoleDebug.println("Provisioning mode initialize");
        xTouchConfig.xTouchProvisioningMode = true;
        if (!cloud.hasAuthTokens()) 
        {
          xtouch_webserver_begin();
          String gotoCode = "Настройка по адресу " + WiFi.localIP().toString();
          lv_label_set_text(introScreenCaption, gotoCode.c_str());
          lv_obj_set_style_text_color(introScreenCaption, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
          lv_timer_handler();
        }
        else
        {
          cloud.loadAuthTokens();

          if (!cloud.isPaired())
          {
            cloud.selectPrinter();
          }
          else
          {
            cloud.loadPair();
          }
          xtouch_cloud_mqtt_setup();
        }

    }else if(xtouch_filesystem_exist(SD, xtouch_paths_config)){
        ConsoleDebug.println("Lan only mode initialize");
        xTouchConfig.xTouchLanOnlyMode = true;
        xtouch_local_mqtt_setup();
    }


#if defined(__XTOUCH_SCREEN_28__)
  ConsoleDebug.println("XTOUCH_NEO_PIXEL_INIT");
  if (board == lgfx::boards::board_t::board_Guition_ESP32_2432W328R || 
      board == lgfx::boards::board_t::board_Guition_ESP32_2432W328C || 
      board == lgfx::boards::board_t::board_Sunton_ESP32_3248S035C ||
      board == lgfx::boards::board_t::board_Sunton_ESP32_3248S035R)
  {
    ConsoleDebug.println("LED 21");
    xTouchConfig.xTouchNeoPixelPinValue = 21;
  }
  else if (board == lgfx::boards::board_t::board_Sunton_ESP32_2432S028_7789 ||
           board == lgfx::boards::board_t::board_Sunton_ESP32_2432S028_9341 )
  {
    ConsoleDebug.println("LED 27");
    xTouchConfig.xTouchNeoPixelPinValue = 27;
  }
#elif defined(__XTOUCH_SCREEN_50__)
  ConsoleDebug.println("LED 17");
  xTouchConfig.xTouchNeoPixelPinValue = 17;
#endif

  xtouch_neo_pixel_timer_init(xTouchConfig.xTouchNeoPixelPinValue);
  xtouch_chamber_timer_init();
  xtouch_screen_startScreenTimer();

// #if defined(__XTOUCH_SCREEN_28__)
//   xtouch_m5stack_buttons_setup((int)tft.getBoard());
// #endif
}

#ifdef XTOUCH_DEBUG_VERBOSE
static uint32_t s_last_heap_log_ms = 0;
#define HEAP_LOG_INTERVAL_MS 5000
#endif

void loop()
{
  ArduinoOTA.handle();
  lv_timer_handler();
  lv_task_handler();
  if (xTouchConfig.xTouchLanOnlyMode ||cloud.loggedIn)
    xtouch_cloud_mqtt_loop();

  if (xtouch_ota_update_flag)
  {
    xtouch_ota_update_flag = false;
    lv_msg_send(XTOUCH_SETTINGS_OTA_UPDATE_NOW, NULL);
  }
  
  // キャラクターアニメーション処理（millis()ベース、タイマー不要）
  xtouch_events_onCharacterAnimation();

// #if defined(__XTOUCH_SCREEN_28__)
//   xtouch_m5stack_buttons_loop();
// #endif

#ifdef XTOUCH_DEBUG_VERBOSE
  if (millis() - s_last_heap_log_ms >= HEAP_LOG_INTERVAL_MS)
  {
    s_last_heap_log_ms = millis();
    ConsoleVerbose.printf("[xPTouch][V][Heap] free=%u max_alloc=%u\n", (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getMaxAllocHeap());
  }
#endif

  delay(10);
}
