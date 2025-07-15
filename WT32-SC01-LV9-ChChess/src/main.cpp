#define LGFX_USE_V1
#define LGFX_WT32_SC01
#define LGFX_AUTODETECT

#include "LvChess.h"
#include <Arduino.h>
#include <LovyanGFX.hpp>
#include <lvgl.h>

static LGFX tft;

#define LV_DISP_HOR_RES 320
#define LV_DISP_VER_RES 480

static const uint32_t buf_size =
    LV_DISP_HOR_RES * LV_DISP_VER_RES * sizeof(lv_color_t);
static lv_color_t *dis_buf1;

void inline lv_disp_init() {
  auto *disp = lv_display_create(LV_DISP_HOR_RES, LV_DISP_VER_RES);
  auto f_disp = [](lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    auto dat = (lgfx::rgb565_t *)px_map;
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushPixelsDMA(dat, w * h);
    lv_display_flush_ready(disp);
  };
  lv_display_set_flush_cb(disp, f_disp);
  auto mode = LV_DISPLAY_RENDER_MODE_PARTIAL;
  dis_buf1 = (lv_color_t *)ps_malloc(buf_size);
  lv_display_set_buffers(disp, dis_buf1, nullptr, buf_size, mode);
}

void inline lv_touch_init() {
  auto *indev_touchpad = lv_indev_create();
  lv_indev_set_type(indev_touchpad, LV_INDEV_TYPE_POINTER);
  auto f_touch = [](lv_indev_t *indev_drv, lv_indev_data_t *data) {
    uint16_t touchX, touchY;
    data->state = LV_INDEV_STATE_RELEASED;
    if (tft.getTouch(&touchX, &touchY)) {
      data->state = LV_INDEV_STATE_PRESSED;
      data->point.x = touchX;
      data->point.y = touchY;
    }
  };
  lv_indev_set_read_cb(indev_touchpad, f_touch);
}

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.initDMA();
  tft.setRotation(0);
  tft.setBrightness(50);
  lv_init();
  lv_tick_set_cb((lv_tick_get_cb_t)millis);
  lv_disp_init();
  lv_touch_init();
  lv_chess_start();
}

void loop() { lv_timer_handler(); }