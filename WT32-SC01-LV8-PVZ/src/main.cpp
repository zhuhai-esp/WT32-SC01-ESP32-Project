#define LGFX_USE_V1
#define LGFX_WT32_SC01
#define LGFX_AUTODETECT

#include "gui/pvz.h"
#include <Arduino.h>
#include <LovyanGFX.hpp>
#include <lvgl.h>

#define LV_DISP_HOR_RES 480
#define LV_DISP_VER_RES 320

LGFX tft;

static const uint32_t buf_size = LV_DISP_HOR_RES * 10;
static lv_color_t dis_buf1[buf_size];
static lv_color_t dis_buf2[buf_size];

void inline lv_disp_init() {
  static lv_disp_draw_buf_t draw_buf;
  static lv_disp_drv_t disp_drv;
  lv_disp_draw_buf_init(&draw_buf, dis_buf1, dis_buf2, buf_size);
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = LV_DISP_HOR_RES;
  disp_drv.ver_res = LV_DISP_VER_RES;
  disp_drv.flush_cb = [](lv_disp_drv_t *disp, const lv_area_t *area,
                         lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
    tft.endWrite();

    lv_disp_flush_ready(disp);
  };
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);
}

void inline lv_touch_init() {
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = [](lv_indev_drv_t *_, lv_indev_data_t *data) {
    uint16_t touchX, touchY;
    bool touched = tft.getTouch(&touchX, &touchY);
    if (!touched) {
      data->state = LV_INDEV_STATE_REL;
    } else {
      data->state = LV_INDEV_STATE_PR;
      data->point.x = touchX;
      data->point.y = touchY;
    }
  };
  lv_indev_drv_register(&indev_drv);
}

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.initDMA();
  tft.setRotation(1);
  tft.setBrightness(10);
  lv_init();
  lv_disp_init();
  lv_touch_init();

  pvz_start();
}

void loop() { lv_timer_handler(); }
