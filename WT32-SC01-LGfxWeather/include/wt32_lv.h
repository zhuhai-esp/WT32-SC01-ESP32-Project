#ifndef __WT32_LV_H__
#define __WT32_LV_H__

#define LGFX_USE_V1

#include "lgfx.h"
#include <Arduino.h>

LGFX tft;

inline void init_wt32_gfx() {
  tft.init();
  tft.initDMA();
  tft.startWrite();
  tft.setBrightness(20);
  tft.clearDisplay(TFT_BLACK);
}

inline void show_center_msg(const char *msg) {
  tft.setColor(TFT_WHITE);
  tft.drawCenterString(msg, screenWidth / 2, screenHeight / 2, &fonts::AsciiFont8x16);
}

#endif