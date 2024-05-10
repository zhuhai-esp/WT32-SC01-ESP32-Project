#ifndef __WT32_LV_H__
#define __WT32_LV_H__

#define LGFX_USE_V1
#define LGFX_WT32_SC01
#define LGFX_AUTODETECT

#include <Arduino.h>
#include <LovyanGFX.hpp>

#define TFT_WIDTH 480
#define TFT_HEIGHT 320

LGFX tft;

inline void init_wt32_gfx() {
  tft.init();
  tft.setRotation(1);
  tft.setBrightness(20);
}

inline void show_center_msg(const char *msg) {
  tft.setColor(TFT_WHITE);
  tft.drawCenterString(msg, TFT_WIDTH / 2, TFT_HEIGHT / 2,
                       &fonts::AsciiFont8x16);
}

#endif