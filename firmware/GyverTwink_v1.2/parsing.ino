#define MAX_UDP_PACKET 30
byte ubuf[MAX_UDP_PACKET];

void replyStart() {
  udp.beginPacket(udp.remoteIP(), udp.remotePort());
  udp.write("GT");
}
void replyData(byte* data, byte size) {
  udp.write(data, size);
}
void replyEnd() {
  udp.endPacket();
}
void reply(byte* data, byte size) {
  replyStart();
  replyData(data, size);
  replyEnd();
}

void parsing() {
  static uint32_t tmr;
  if (udp.parsePacket()) {
    if (millis() - tmr < 50) {  // "антидребезг" приёма
      udp.read(ubuf, MAX_UDP_PACKET);
      return;
    }
    tmr = millis();
    int n = udp.read(ubuf, MAX_UDP_PACKET);
    ubuf[n] = 0;
    if (ubuf[0] != 'G' || ubuf[1] != 'T') return;
    /*for (int i = 2; i < n; i++) {
      DEBUG(ubuf[i]);
      DEBUG(',');
      }
      DEBUGLN();*/
    byte answ[10];

    switch (ubuf[2]) {
      case 0:   // запрос IP
        delay(myIP[3] * 2);
        answ[0] = 0;
        answ[1] = myIP[3];
        reply(answ, 2);
        break;

      case 1:   // запрос настроек
        answ[0] = 1;
        answ[1] = cfg.ledAm;
        answ[2] = cfg.strAm;
        answ[3] = cfg.power;
        answ[4] = cfg.bright;
        answ[5] = cfg.autoCh;
        answ[6] = cfg.rndCh;
        answ[7] = cfg.prdCh;
        answ[8] = cfg.turnOff;
        answ[9] = cfg.offTmr;
        reply(answ, 10);
        break;

      case 2:   // приём настроек
        forceTmr.stop();
        switch (ubuf[3]) {
          case 0: 
            cfg.ledAm = ubuf[4];
            FastLED.clear(true);
                        
            if(cfg.strAm<2)
              FastLED.addLeds<LED_TYPE, D1, LED_ORDER>(leds, 0*cfg.ledAm, cfg.ledAm).setCorrection(TypicalLEDStrip);
            if(cfg.strAm<3)  
              FastLED.addLeds<LED_TYPE, D2, LED_ORDER>(leds, 1*cfg.ledAm, cfg.ledAm).setCorrection(TypicalLEDStrip);
            if(cfg.strAm<4)
              FastLED.addLeds<LED_TYPE, D4, LED_ORDER>(leds, 2*cfg.ledAm, cfg.ledAm).setCorrection(TypicalLEDStrip);

            break;
          case 9: 
            cfg.strAm = ubuf[4];
            FastLED.clear(true);
                        
            if(cfg.strAm<2)
              FastLED.addLeds<LED_TYPE, D1, LED_ORDER>(leds, 0*cfg.ledAm, cfg.ledAm).setCorrection(TypicalLEDStrip);
            if(cfg.strAm<3)  
              FastLED.addLeds<LED_TYPE, D2, LED_ORDER>(leds, 1*cfg.ledAm, cfg.ledAm).setCorrection(TypicalLEDStrip);
            if(cfg.strAm<4)
              FastLED.addLeds<LED_TYPE, D4, LED_ORDER>(leds, 2*cfg.ledAm, cfg.ledAm).setCorrection(TypicalLEDStrip);
            break;
          case 1: cfg.power = ubuf[4];
            break;
          case 2: cfg.bright = ubuf[4];
            break;
          case 3: cfg.autoCh = ubuf[4];
            if (cfg.autoCh) switchTmr.restart();
            else switchTmr.stop();
            break;
          case 4: cfg.rndCh = ubuf[4];
            break;
          case 5: cfg.prdCh = ubuf[4];
            switchTmr.setPrd(cfg.prdCh * 60000ul);
            if (cfg.autoCh) switchTmr.restart();
            break;
          case 6:   // нехт эффект
            switchEff();
            if (cfg.autoCh) switchTmr.restart();
            return;
            break;
          case 7:
            cfg.turnOff = ubuf[4];
            if (cfg.turnOff) offTmr.restart();
            else offTmr.stop();
            break;
          case 8:
            cfg.offTmr = ubuf[4];
            offTmr.setPrd(cfg.offTmr * 60000ul);
            if (cfg.turnOff) offTmr.restart();
            break;
        }
        if (!cfg.power) {
          FastLED.setBrightness(0);
          FastLED.show();
        }
        EEcfg.update();
        break;

      case 3:
        switch (ubuf[3]) {
          case 0:   // запуск калибровки
            DEBUGLN("Calibration start");
            calibF = true;
            FastLED.clear(true);
            FastLED.setBrightness(0);
            FastLED.show();
            break;

          case 1:   // следующий лед
            {
              int curLed = ubuf[4] * 100 + ubuf[5];
              if (curLed > 0) {
                xy[curLed - 1][0] = ubuf[6];
                xy[curLed - 1][1] = ubuf[7];
              }
              FastLED.clear(true);
              leds[curLed] = CRGB::White;
              FastLED.setBrightness(200);
              FastLED.show();
            }
            break;

          case 2:   // калибровка окончена
            DEBUGLN("Finished");
            calibF = false;
            FastLED.clear(true);
            FastLED.setBrightness(0);
            FastLED.show();
            EExy.updateNow();

            mm.minY = 255;
            mm.maxY = 0;
            mm.minX = 255;
            mm.maxX = 0;
            for (int i = 0; i < cfg.strAm * cfg.ledAm; i++) {
              mm.minX = min(mm.minX, xy[i][0]);
              mm.maxX = max(mm.maxX, xy[i][0]);
              mm.minY = min(mm.minY, xy[i][1]);
              mm.maxY = max(mm.maxY, xy[i][1]);
            }
            mm.w = mm.maxX - mm.minX;
            mm.h = mm.maxY - mm.minY;
            EEmm.updateNow();
            break;
        }
        break;

      case 4:   // управление эффектами
        forceTmr.restart();
        EEeff.update();
        switch (ubuf[3]) {
          case 0:   // выбор эффекта в дропе
            forceEff = ubuf[4];
            answ[0] = 4;
            answ[1] = effs[forceEff].fav;
            answ[2] = effs[forceEff].scale;
            answ[3] = effs[forceEff].speed;
            reply(answ, 4);
            loadingFlag = true;
            DEBUG("forceEff ");
            DEBUGLN(forceEff);
            
            break;
          case 1:   // флажок избранное
            effs[forceEff].fav = ubuf[4];
            break;
          case 2:   // масштаб
            effs[forceEff].scale = ubuf[4];
            loadingFlag = true;
            break;
          case 3:   // скорость
            effs[forceEff].speed = ubuf[4];
            loadingFlag = true;
            break;
        }
        break;
      case 5:   // рисование
        switch (ubuf[3]) {
          case 0:   // переход в режим рисования
            DEBUGLN("Paint start");
            paintF = true;
            answ[0] = 5;
            answ[1] = mm.w;
            answ[2] = mm.h;
            DEBUG(mm.w);DEBUG(" ");DEBUGLN(mm.h);
            DEBUG(mm.minX);DEBUG(" ");DEBUGLN(mm.maxX);
            DEBUG(mm.minY);DEBUG(" ");DEBUGLN(mm.maxY);
            
            reply(answ, 3);
            
            for(int i=0;i<300;i+=50) {
              answ[0] = 6;
              answ[1] = i;
              answ[2] = 50;
              replyStart();
              replyData(answ, 3);
              replyData((byte *)(xy + i * 2), 50 * 2);
              replyEnd();
            }

            FastLED.clear(true);
            FastLED.setBrightness(0);
            FastLED.show();
          break;

          case 1:   // рисуем пиксель
            {
              CRGB c = CRGB::Black;
              if(ubuf[7]==1) {
                c = CRGB::White;
              }
              drawPixelXYD(ubuf[4], ubuf[5], ubuf[6], c);
              FastLED.setBrightness(200);
              FastLED.show();
            }
            break;
          
          case 2:   // завершение режима рисования
            DEBUGLN("Paint end");
            paintF = false;
          break;
        }
        break;
        case 6:   // пикели
        switch (ubuf[3]) {
          case 0:   // передать расположение пикселей
            DEBUGLN("pixels part");
            answ[0] = 6;
            answ[1] = ubuf[4];
            answ[2] = ubuf[5];
            for (int i = ubuf[4]; i < ubuf[4] + ubuf[5]; i++) {
              DEBUG(i);DEBUG(" ");
              DEBUG(xy[i][0]);DEBUG(" ");
              DEBUGLN(xy[i][1]);
            }
            replyStart();
            replyData(answ, 3);
            replyData((byte *)(xy + ubuf[4] * 2), ubuf[5] * 2);
            replyEnd();
          break;
          
        }
        break;

    }
  }
}
