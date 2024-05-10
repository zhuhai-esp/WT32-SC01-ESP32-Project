#define LGFX_WT32_SC01
#define LGFX_AUTODETECT

#include <LovyanGFX.hpp>

static LGFX lcd;
static LGFX_Sprite sprite(&lcd);

void setup() {
  Serial.begin(115200);
  lcd.init();
  if (lcd.width() < lcd.height()) {
    lcd.setRotation(lcd.getRotation() ^ 1);
  }
  lcd.setBrightness(20);
  lcd.setColor(TFT_BLUE);
  lcd.fillCircle(100, 50, 30);

  sprite.setColorDepth(24);
  sprite.createSprite(65, 65);
  sprite.drawRect(0, 0, 65, 65, TFT_CYAN);
  sprite.drawCircle(30, 20, 20, TFT_GOLD);
  sprite.pushSprite(160, 80);
  sprite.deleteSprite();
}

void loop() {}