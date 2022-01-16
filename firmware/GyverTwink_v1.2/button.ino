void button() {
  static bool dir = 1;

  btn.tick();
  if (btn.hasClicks(1)) {
    DEBUGLN("1x click - power");
    cfg.power = !cfg.power;
    if (!cfg.power) {
        FastLED.setBrightness(0);
        FastLED.show();
    }
    EEcfg.update();
  }
  
  if (btn.hasClicks(2)) {
    DEBUGLN("2x click - next");
    switchEff();
  }
  
  if (btn.step()) {
    DEBUGLN("change bright");
    if (cfg.power) {
      // меняем безопасно
      int bri = cfg.bright;
      bri += dir ? 30 : -30;
      bri = constrain(bri, 0, 255);      
      cfg.bright = bri;
      if (bri == 255) {
        FastLED.setBrightness(0);
        FastLED.show();
        delay(100);
        FastLED.setBrightness(bri);
        FastLED.show();
      }
      EEcfg.update();
    }
  }
  
  if (btn.releaseStep()) {
    DEBUGLN("change dir");
    dir = !dir;
  }
}
