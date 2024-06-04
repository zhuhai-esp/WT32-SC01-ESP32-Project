#define LGFX_USE_V1
#define LGFX_WT32_SC01
#define LGFX_AUTODETECT
#define USE_UI

#include <Arduino.h>
#include <LovyanGFX.hpp>
#include <lvgl.h>

#ifdef USE_UI
#include "ui/ui.h"
#endif

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

int wukong, x = 0;
int Kong, y = 0;
int value1 = -10, value2 = 110;

void anu_hit();
void wukong_hit();

static bool btn1 = false;
static bool btn2 = false;
static bool restart = true;

void buttonClicked(lv_event_t *e) {
  lv_event_code_t event_code = lv_event_get_code(e);
  lv_obj_t *target = lv_event_get_target(e);
  if (target == ui_button1) {
    btn1 = true;
  }
  if (target == ui_button2) {
    btn2 = true;
  }
}

void buttonReleased(lv_event_t *e) {
  lv_event_code_t event_code = lv_event_get_code(e);
  lv_obj_t *target = lv_event_get_target(e);
  if (target == ui_button1) {
    btn1 = false;
  }
  if (target == ui_button2) {
    btn2 = false;
  }
}

void restartGame(lv_event_t *e) {
  lv_event_code_t event_code = lv_event_get_code(e);
  lv_obj_t *target = lv_event_get_target(e);
  if (event_code == LV_EVENT_CLICKED) {
    wukong, x = 0;
    Kong, y = 0;
    value1 = -10, value2 = 110;
    restart = false;
  }
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

  ui_init();

  lv_img_set_src(ui_Image6, &ui_img_0_png);
  lv_slider_set_value(ui_Slider1, value1, LV_ANIM_OFF);
  lv_slider_set_value(ui_Slider2, value2, LV_ANIM_OFF);
  lv_timer_handler();
}

void loop() {
  wukong = btn1;
  Kong = btn2;

  if (wukong == 1)
    x += 1;
  else
    x = 0;
  if (Kong == 0)
    y += 1;
  else
    y = 0;
  if (x == 1) {
    wukong_hit();
    value1 += 10;
    lv_slider_set_value(ui_Slider1, value1, LV_ANIM_OFF);
    lv_timer_handler();
    Serial.println(value1);
    if (value1 == 100) {
      lv_img_set_src(ui_Image6, &ui_img_ko_png);
      lv_obj_set_pos(ui_Image1, -19, 12);
      lv_obj_set_pos(ui_Image3, 158, 86);
      lv_img_set_angle(ui_Image3, 900);
      lv_obj_clear_flag(ui_restartButton, LV_OBJ_FLAG_HIDDEN);
      delay(500);
      lv_timer_handler();
      while (restart) {
        lv_timer_handler();
      }
      restart = true;
      lv_disp_load_scr(ui_Screen2);
      lv_obj_add_flag(ui_restartButton, LV_OBJ_FLAG_HIDDEN);
    }
  }
  if (y == 1) {
    anu_hit();
    value2 -= 10;
    lv_slider_set_value(ui_Slider2, value2, LV_ANIM_OFF);
    lv_timer_handler();
    Serial.println(value2);
    if (value2 == 0) {
      lv_img_set_src(ui_Image6, &ui_img_ko_png);
      lv_obj_set_pos(ui_Image3, -1, -8);
      lv_obj_set_pos(ui_Image1, -165, 86);
      lv_img_set_angle(ui_Image1, -900);
      lv_obj_clear_flag(ui_restartButton, LV_OBJ_FLAG_HIDDEN);
      delay(500);
      lv_timer_handler();

      while (restart) {
        lv_timer_handler();
      }
      lv_disp_load_scr(ui_Screen2);
      lv_obj_add_flag(ui_restartButton, LV_OBJ_FLAG_HIDDEN);
      restart = true;
    }
  }
  lv_timer_handler();
}

void wukong_hit() {
  lv_img_set_src(ui_Image7, &ui_img_0_png);
  lv_obj_set_pos(ui_Image7, -81, 39);
  lv_img_set_src(ui_Image1, &ui_img_1_png);
  lv_timer_handler(); /* let the GUI do its work */
  delay(55);
  lv_img_set_src(ui_Image1, &ui_img_2_png);
  lv_timer_handler(); /* let the GUI do its work */
  delay(55);
  lv_img_set_src(ui_Image1, &ui_img_3_png);
  lv_timer_handler(); /* let the GUI do its work */
  delay(55);
  lv_img_set_src(ui_Image7, &ui_img_7_png);
  lv_obj_set_pos(ui_Image7, -81, 39);
  lv_img_set_src(ui_Image1, &ui_img_4_png);
  lv_timer_handler(); /* let the GUI do its work */
  delay(55);
  lv_obj_set_pos(ui_Image7, -2, 39);
  lv_img_set_src(ui_Image7, &ui_img_8_png);
  lv_img_set_src(ui_Image1, &ui_img_5_png);
  lv_timer_handler(); /* let the GUI do its work */
  delay(55);
  lv_obj_set_pos(ui_Image7, 70, 39);
  lv_img_set_src(ui_Image7, &ui_img_8_png);
  lv_img_set_src(ui_Image1, &ui_img_6_png);
  lv_timer_handler(); /* let the GUI do its work */
  delay(55);
  lv_obj_set_pos(ui_Image7, 135, 39);
  lv_img_set_src(ui_Image7, &ui_img_9_png);
  lv_img_set_src(ui_Image1, &ui_img_6_png);
  lv_timer_handler(); /* let the GUI do its work */
  delay(55);
  lv_img_set_src(ui_Image7, &ui_img_0_png);
  lv_img_set_src(ui_Image1, &ui_img_6_png);
  lv_timer_handler(); /* let the GUI do its work */
  delay(55);
}

void anu_hit() {
  lv_img_set_src(ui_Image4, &ui_img_0_png);
  lv_obj_set_pos(ui_Image4, 104, 59);
  lv_obj_set_pos(ui_Image3, 158, 36);
  lv_img_set_src(ui_Image3, &ui_img_11_png);
  lv_timer_handler(); /* let the GUI do its work */
  delay(55);
  lv_img_set_src(ui_Image3, &ui_img_12_png);
  lv_timer_handler(); /* let the GUI do its work */
  delay(55);
  lv_img_set_src(ui_Image3, &ui_img_13_png);
  lv_timer_handler(); /* let the GUI do its work */
  delay(55);
  lv_img_set_src(ui_Image3, &ui_img_14_png);
  lv_timer_handler(); /* let the GUI do its work */
  delay(55);
  lv_img_set_src(ui_Image3, &ui_img_15_png);
  lv_timer_handler(); /* let the GUI do its work */
  delay(55);
  lv_img_set_src(ui_Image3, &ui_img_16_png);
  lv_timer_handler(); /* let the GUI do its work */
  delay(55);
  lv_img_set_src(ui_Image4, &ui_img_19_png);
  lv_obj_set_pos(ui_Image4, 64, 39);
  lv_img_set_src(ui_Image3, &ui_img_18_png);
  lv_timer_handler(); /* let the GUI do its work */
  delay(55);
  lv_obj_set_pos(ui_Image4, 32, 39);
  lv_img_set_src(ui_Image4, &ui_img_20_png);
  lv_img_set_src(ui_Image3, &ui_img_18_png);
  lv_timer_handler(); /* let the GUI do its work */
  delay(55);
  lv_obj_set_pos(ui_Image4, -50, 39);
  lv_img_set_src(ui_Image4, &ui_img_21_png);
  lv_img_set_src(ui_Image3, &ui_img_16_png);
  lv_timer_handler(); /* let the GUI do its work */
  delay(55);
  lv_obj_set_pos(ui_Image4, -135, 39);
  lv_img_set_src(ui_Image4, &ui_img_21_png);
  lv_img_set_src(ui_Image3, &ui_img_15_png);
  lv_timer_handler(); /* let the GUI do its work */
  delay(55);
  lv_img_set_src(ui_Image4, &ui_img_0_png);
  lv_img_set_src(ui_Image3, &ui_img_11_png);
  lv_timer_handler(); /* let the GUI do its work */
  delay(55);
}
