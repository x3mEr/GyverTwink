void portalRoutine() {
  // запускаем portal
  portalStart();
  DEBUGLN("Portal start");

  // ждём действий пользователя, мигаем
  while (!portalTick()) fader(CRGB::Blue);

  // если это 1 connect, 2 ap, 3 local, обновляем данные в епр
  if (portalStatus() <= 3) EEwifi.updateNow();

  DEBUG("Portal status: ");
  DEBUGLN(portalStatus());
}

void startStrip() {
  addStripLoop<1>(); // start from the first strip

  FastLED.clear(true);
  // выводим ргб
  for(uint8_t Idx = 0; Idx < cfg.strAm; ++Idx) {
    leds[Idx * cfg.ledAm + 0] = CRGB::Red;
    leds[Idx * cfg.ledAm + 1] = CRGB::Green;
    leds[Idx * cfg.ledAm + 2] = CRGB::Blue;
  }

  FastLED.setBrightness(cfg.bright);
  FastLED.show();
}

bool checkButton() {
  uint32_t tmr = millis();
  while (millis() - tmr < 2000) {
    btn.tick();
    if (btn.state()) return true;
  }
  return false;
}

void setupAP() {
  DEBUGLN("AP Mode");
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAP(GT_AP_SSID, GT_AP_PASS);
  myIP = WiFi.softAPIP();
  server.begin();
  fadeBlink(CRGB::Magenta);
}

void setupSTA() {
  DEBUGLN("Connecting to AP... ");
  //Serial.setDebugOutput(true);
  WiFi.softAPdisconnect();
  WiFi.disconnect();
  //WiFi.mode(WIFI_STA);
  Serial.printf("Wi-Fi mode set to WIFI_STA %s\n", WiFi.mode(WIFI_STA) ? "" : "Failed!");\
  WiFi.begin(portalCfg.SSID, portalCfg.pass);
  Serial.printf("Connection status: %d\n", WiFi.status());
  //WiFi.printDiag(Serial);
  //while (WiFi.waitForConnectResult() != WL_CONNECTED);
  uint32_t tmr = millis();
  bool state = false;
  while (millis() - tmr < 15000) {
    if (WiFi.waitForConnectResult() == WL_CONNECTED) {
      fadeBlink(CRGB::Green);
      DEBUGLN("ok");
      myIP = WiFi.localIP();
      return;
    }
    fader(CRGB::Yellow);
    yield();
  }
  fadeBlink(CRGB::Red);
  DEBUGLN("fail");
  setupAP();
}
