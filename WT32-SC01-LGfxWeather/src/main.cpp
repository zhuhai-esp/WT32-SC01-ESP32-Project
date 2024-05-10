#include "web_api.h"
#include "wt32_lv.h"
#include "wt32_wifi.h"
#include <Arduino.h>

using namespace std;

#define FONT16 &fonts::efontCN_16_b

long check1s = 0;
long check5ms = 0;
long check5min = 0;

static LGFX_Sprite spTime(&tft);
static LGFX_Sprite spWeather(&tft);

static json city_info;
static json weather;

inline void create_draw_sprites() {
  spTime.createSprite(240, 160);
  spTime.setColorDepth(8);
  spWeather.createSprite(240, 160);
  spTime.setColorDepth(8);
}

inline void showClientIP() {
  sprintf(buf, "IP: %s", WiFi.localIP().toString().c_str());
  tft.setColor(TFT_WHITE);
  tft.drawRightString(buf, screenWidth - 10, screenHeight - 26, FONT16);
}

inline void showCurrentTime() {
  struct tm info;
  getLocalTime(&info);
  strftime(buf, 32, "%F", &info);
  spTime.drawString(buf, 10, 10, &fonts::Font4);
  strftime(buf, 32, "%T", &info);
  spTime.drawString(buf, 10, 48, &fonts::Font4);
  spTime.pushSprite(0, 0);
}

inline void updateWeatherInfo() {
  weather = get_now_weather_info(city_info["cityId"]);
  json ip_info = city_info["ip"];
  string province = ip_info["province"], city = ip_info["city"],
         district = ip_info["district"], condition = weather["CONDITIONSTEXT"],
         wind = weather["WIND"];
  int temp = weather["TEMP"], winp = weather["WINP"], humi = weather["HUMI"];
  sprintf(buf, "%s %s %s", province.c_str(), city.c_str(), district.c_str());
  spWeather.drawString(buf, 10, 10, FONT16);
  sprintf(buf, "%s %d°C", condition.c_str(), temp);
  spWeather.drawString(buf, 10, 30, FONT16);
  sprintf(buf, "%s %d级 | 湿度 %d%%", wind.c_str(), winp, humi);
  spWeather.drawString(buf, 10, 50, FONT16);
  spWeather.pushSprite(0, 160);
}

void setup() {
  Serial.begin(115200);
  init_wt32_gfx();
  create_draw_sprites();
  autoConfigWifi();
  const int timeZone = 8 * 3600;
  configTime(timeZone, 0, "ntp6.aliyun.com", "cn.ntp.org.cn", "ntp.ntsc.ac.cn");
  while (time(nullptr) < 8 * 3600 * 2) {
    delay(500);
  }
  setupOTAConfig();
  tft.clear(TFT_BLACK);
  showClientIP();
  city_info = get_cur_city_info();
  updateWeatherInfo();
}

void loop() {
  auto ms = millis();
  if (ms - check1s > 1000) {
    check1s = ms;
    ArduinoOTA.handle();
    showCurrentTime();
  }
  if (ms - check5min > 5 * 60 * 1000) {
    check5min = ms;
    updateWeatherInfo();
  }
}