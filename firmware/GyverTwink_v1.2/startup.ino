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
  FastLED.addLeds<LED_TYPE, D1, LED_ORDER>(leds, 0*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, D2, LED_ORDER>(leds, 1*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, D4, LED_ORDER>(leds, 2*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  
  FastLED.clear(true);
  // выводим ргб
  leds[0] = CRGB::Red;
  leds[1] = CRGB::Green;
  leds[2] = CRGB::Blue;

  leds[NUM_LEDS_PER_STRIP+0] = CRGB::Red;
  leds[NUM_LEDS_PER_STRIP+1] = CRGB::Green;
  leds[NUM_LEDS_PER_STRIP+2] = CRGB::Blue;

  leds[2*NUM_LEDS_PER_STRIP+0] = CRGB::Red;
  leds[2*NUM_LEDS_PER_STRIP+1] = CRGB::Green;
  leds[2*NUM_LEDS_PER_STRIP+2] = CRGB::Blue;
 
  FastLED.setBrightness(50);
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
