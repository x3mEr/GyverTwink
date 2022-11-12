void effectsTick()
{
  switch (forceEff)
      {
        case EFF_SPARKLES:       sparklesRoutine();           break;
        case EFF_RAINBOW_VER:    rainbowVerticalRoutine();    break;
        case EFF_RAINBOW_HOR:    rainbowHorizontalRoutine();  break;
        case EFF_RAINBOW_DIAG:   rainbowDiagonalRoutine();    break;
/*        case EFF_FIRE:           fireRoutine(true);           break;
        case EFF_WHITTE_FIRE:    fireRoutine(false);          break;
        case EFF_COLORS:         colorsRoutine();             break;
        case EFF_MADNESS:        madnessNoiseRoutine();       break;
        case EFF_CLOUDS:         cloudsNoiseRoutine();        break;
        case EFF_LAVA:           lavaNoiseRoutine();          break;
        case EFF_PLASMA:         plasmaNoiseRoutine();        break;
        case EFF_RAINBOW:        rainbowNoiseRoutine();       break;
        case EFF_RAINBOW_STRIPE: rainbowStripeNoiseRoutine(); break;
        case EFF_ZEBRA:          zebraNoiseRoutine();         break;
        case EFF_FOREST:         forestNoiseRoutine();        break;
        case EFF_OCEAN:          oceanNoiseRoutine();         break;
        case EFF_COLOR:          colorRoutine();              break;
        case EFF_SNOW:           snowRoutine();               break;
        case EFF_SNOWSTORM:      snowStormRoutine();          break;
        case EFF_STARFALL:       starfallRoutine();           break;
        case EFF_MATRIX:         matrixRoutine();             break;
        case EFF_LIGHTERS:       lightersRoutine();           break;
        case EFF_LIGHTER_TRACES: ballsRoutine();              break;
        case EFF_PAINTBALL:      lightBallsRoutine();         break;
        case EFF_CUBE:           ballRoutine();               break;
        case EFF_WHITE_COLOR:    whiteColorStripeRoutine();   break;*/
        default:                                              break;
      }
      FastLED.setBrightness(cfg.bright);
      FastLED.show();
}


void drawPixelXYD(int32_t x, int32_t y, int8_t dist, CRGB color) {
  for (int i = 0; i < cfg.strAm * cfg.ledAm; i++) {
    if (xy[i][0]-dist <= x && x <= xy[i][0] + dist) {
      if (xy[i][1]-dist <= y && y <= xy[i][1] + dist) {
        leds[i] = color;
      }
    }
  }
}

void drawPixelXY(int32_t x, int32_t y, CRGB color) {
  int32_t thisPixel = getPixelNumber(x, y);
  if(thisPixel != -1) {
    leds[thisPixel] = color;
  }
}
uint32_t getPixColorXY(int32_t x, int32_t y) {
  int32_t thisPixel = getPixelNumber(x, y);
  if(thisPixel != -1) {
    return (((uint32_t)leds[thisPixel].r << 16) | ((uint32_t)leds[thisPixel].g << 8 ) | (uint32_t)leds[thisPixel].b);
  }
  return 0;

}

int ledIdxByXY(int32_t x, int32_t y, int8_t dist) {
  int idx=-1;
  for (int32_t i = 0; i < cfg.strAm * cfg.ledAm; i++) {
    if (xy[i][0]-dist <= x && x <= xy[i][0] + dist) {
      if (xy[i][1]-dist <= y && y <= xy[i][1] + dist) {
        idx = i;
        break;
      }
    }
  }
  return idx;
}


// ------------- конфетти --------------
#define FADE_OUT_SPEED        (70U)                         // скорость затухания
void sparklesRoutine()
{
  for (int32_t i = 0; i < effs[EFF_SPARKLES].scale / 10.0F; i++)
  {
    
    int32_t thisPixel = random(0, cfg.strAm * cfg.ledAm);
    if (getPixColor(thisPixel) == 0U)
    {
      leds[thisPixel] = CHSV(random(0U, 255U), 255U, 255U);
    }
  }
  fader(effs[EFF_SPARKLES].speed);
}

/*
void sparklesRoutine()
{
  for (uint8_t i = 0; i < effs[EFF_SPARKLES].scale; i++)
  {
    uint8_t x = random(mm.minX, mm.maxX);
    uint8_t y = random(mm.minY, mm.maxY);
    if (getPixColorXY(x, y) == 0U)
    {
      leds[getPixelNumber(x, y)] = CHSV(random(0U, 255U), 255U, 255U);
    }
  }
  fader(FADE_OUT_SPEED);
}
*/ 
// функция плавного угасания цвета для всех пикселей
void fader(uint8_t step)
{
  for (int32_t p = 0; p < cfg.strAm * cfg.ledAm; p++) {
    fadePixel(p, step);
  }
  /*for (uint8_t i = 0U; i < mm.w; i++)
  {
    for (uint8_t j = 0U; j < mm.h; j++)
    {
      fadePixel(i, j, step);
    }
  }*/
}
int32_t getPixelNumber(uint8_t x, uint8_t y) {
  return ledIdxByXY(x, y, 2);
}

uint32_t getPixColor(uint32_t thisPixel) {
  if (thisPixel > cfg.strAm * cfg.ledAm - 1) return 0;
  return (((uint32_t)leds[thisPixel].r << 16) | ((uint32_t)leds[thisPixel].g << 8 ) | (uint32_t)leds[thisPixel].b);
}

void fadePixel(uint8_t i, uint8_t j, uint8_t step)          // новый фейдер
{
  int32_t pixelNum = getPixelNumber(i, j);
  if (getPixColor(pixelNum) == 0U) return;

  if (leds[pixelNum].r >= 30U ||
      leds[pixelNum].g >= 30U ||
      leds[pixelNum].b >= 30U)
  {
    leds[pixelNum].fadeToBlackBy(step);
  }
  else
  {
    leds[pixelNum] = 0U;
  }
}

void fadePixel(int32_t pixelNum, uint8_t step)          // новый фейдер
{
  if (getPixColor(pixelNum) == 0U) return;

  if (leds[pixelNum].r >= 30U ||
      leds[pixelNum].g >= 30U ||
      leds[pixelNum].b >= 30U)
  {
    leds[pixelNum].fadeToBlackBy(step);
  }
  else
  {
    leds[pixelNum] = 0U;
  }
}

void effects() {
  static Timer effTmr(30);
  static uint16_t countP = 0;
  static byte countSkip = 0;
  static byte prevEff = 255;
  static byte fadeCount = 0;

  if (effTmr.ready()) {
    byte thisEffect;

    if (forceTmr.state()) thisEffect = forceEff;
    else thisEffect = curEff;

    // эффект сменился
    if (prevEff != curEff) {
      prevEff = curEff;
      fadeCount = 25;
    }

    byte scale = effs[thisEffect].scale;
    byte speed = effs[thisEffect].speed;
    byte curPal = thisEffect;
    if (curPal >= ACTIVE_PALETTES) curPal -= ACTIVE_PALETTES;

    for (int i = 0; i < cfg.strAm * cfg.ledAm; i++) {
      byte idx;

      if (thisEffect < ACTIVE_PALETTES) {
        // первые ACTIVE_PALETTES эффектов - градиент
        // idx = map(xy[i][1], mm.minY, mm.maxY, 0, 255) + counter;   // прямой градиент
        idx = countP + ((mm.w * xy[i][0] / mm.h) + xy[i][1]) * scale / 100;   // диагональный градиент
      } else {
        // следующие - перлин нойс
        idx = inoise8(xy[i][0] * scale / 10, xy[i][1] * scale / 10, countP);
      }
      CRGB color = ColorFromPalette(paletteArr[curPal], idx, 255, LINEARBLEND);

      // плавная смена эффекта
      // меняется за 25 фреймов
      if (fadeCount) leds[i] = blend(leds[i], color, 40);
      else leds[i] = color;
    }
    if (fadeCount) fadeCount--;

    countP += (speed - 128) / 10;
    FastLED.setBrightness(cfg.bright);
    FastLED.show();
  }
}

// ------------- снегопад ----------
void snowRoutine()
{
  // сдвигаем всё вниз
  for (uint8_t x = mm.minX; x < mm.maxX; x++)
  {
    for (uint8_t y = mm.minY; y < mm.maxY - 1; y++)
    {
      drawPixelXY(x, y, getPixColorXY(x, y + 1U));
    }
  }

   for (uint8_t x = mm.minX; x < mm.maxX; x++)
  {
    // заполняем случайно верхнюю строку
    // а также не даём двум блокам по вертикали вместе быть
    if (getPixColorXY(x, mm.maxY - 2U) == 0U && (random(0, 100 - 90 /* modes[EFF_SNOW].Scale */) == 0U))
      drawPixelXY(x, mm.maxY - 1U, 0xE0FFFF - 0x101010 * random(0, 4));
    else
      drawPixelXY(x, mm.maxY - 1U, 0x000000);
  }
}
// ------------- радуга вертикальная ----------------
uint8_t hue;
void rainbowVerticalRoutine()
{
  hue += effs[EFF_RAINBOW_VER].speed / 10.0F;
  for (uint8_t j = mm.minY; j < mm.maxY; j++)
  {
    CHSV thisColor = CHSV((uint8_t)(hue + j * effs[EFF_RAINBOW_VER].scale / 10.0F), 255, 255);
    for (uint8_t i = mm.minX; i < mm.maxX; i++)
    {
      drawPixelXY(i, j, thisColor);
    }
  }
}
// ------------- радуга горизонтальная ----------------
void rainbowHorizontalRoutine()
{
  hue += effs[EFF_RAINBOW_VER].speed / 10.0F;
  for (uint8_t i = mm.minX; i < mm.maxX; i++)
  {
    CHSV thisColor = CHSV((uint8_t)(hue + i * effs[EFF_RAINBOW_HOR].scale / 10.0F), 255, 255);
    for (uint8_t j = mm.minY; j < mm.maxY; j++)
    {
      drawPixelXY(i, j, thisColor);
    }
  }
}

// ------------- радуга диагональная -------------
void rainbowDiagonalRoutine()
{
  if (loadingFlag)
  {
    loadingFlag = false;
    FastLED.clear();
  }

  hue += effs[EFF_RAINBOW_VER].speed / 10.0F;
  for (uint8_t i = mm.minX; i < mm.maxX; i++)
  {
    for (uint8_t j = mm.minY; j < mm.maxY; j++)
    {
      float twirlFactor = 3.0F * (effs[EFF_RAINBOW_DIAG].scale / 10.0F);      // на сколько оборотов будет закручена матрица, [0..3]
      CRGB thisColor = CHSV((uint8_t)(hue + (float)(mm.w / mm.h * i + j * twirlFactor) * (float)(255 / max(mm.w, mm.h))), 255, 255);
      drawPixelXY(i, j, thisColor);
    }
  }
}



// ------------- метель -------------
#define SNOW_DENSE            (60U)                         // плотность снега
#define SNOW_TAIL_STEP        (100U)                        // длина хвоста
#define SNOW_SATURATION       (0U)                          // насыщенность (от 0 до 255)
void snowStormRoutine()
{
  // заполняем головами комет левую и верхнюю линию
  for (uint8_t i = HEIGHT / 2U; i < HEIGHT; i++)
  {
    if (getPixColorXY(0U, i) == 0U &&
       (random(0, SNOW_DENSE) == 0) &&
        getPixColorXY(0U, i + 1U) == 0U &&
        getPixColorXY(0U, i - 1U) == 0U)
    {
      leds[getPixelNumber(0U, i)] = CHSV(random(0, 200), SNOW_SATURATION, 255U);
    }
  }
  
  for (uint8_t i = 0U; i < WIDTH / 2U; i++)
  {
    if (getPixColorXY(i, HEIGHT - 1U) == 0U &&
       (random(0, map(9/*modes[EFF_SNOWSTORM].Scale*/, 0U, 255U, 10U, 120U)) == 0U) &&
        getPixColorXY(i + 1U, HEIGHT - 1U) == 0U &&
        getPixColorXY(i - 1U, HEIGHT - 1U) == 0U)
    {
      leds[getPixelNumber(i, HEIGHT - 1U)] = CHSV(random(0, 200), SNOW_SATURATION, 255U);
    }
  }

  // сдвигаем по диагонали
  for (uint8_t y = 0U; y < HEIGHT - 1U; y++)
  {
    for (uint8_t x = WIDTH - 1U; x > 0U; x--)
    {
      drawPixelXY(x, y, getPixColorXY(x - 1U, y + 1U));
    }
  }

  // уменьшаем яркость левой и верхней линии, формируем "хвосты"
  for (uint8_t i = HEIGHT / 2U; i < HEIGHT; i++)
  {
    fadePixel(0U, i, SNOW_TAIL_STEP);
  }
  for (uint8_t i = 0U; i < WIDTH / 2U; i++)
  {
    fadePixel(i, HEIGHT - 1U, SNOW_TAIL_STEP);
  }
}
// ------------- матрица ---------------
void matrixRoutine()
{
  for (uint8_t x = 0U; x < WIDTH; x++)
  {
    // заполняем случайно верхнюю строку
    uint32_t thisColor = getPixColorXY(x, HEIGHT - 1U);
    if (thisColor == 0U)
      drawPixelXY(x, HEIGHT - 1U, 0x00FF00 * (random(0, 100 - 90/*modes[EFF_MATRIX].Scale*/) == 0U));
    else if (thisColor < 0x002000)
      drawPixelXY(x, HEIGHT - 1U, 0U);
    else
      drawPixelXY(x, HEIGHT - 1U, thisColor - 0x002000);
  }

  // сдвигаем всё вниз
  for (uint8_t x = 0U; x < WIDTH; x++)
  {
    for (uint8_t y = 0U; y < HEIGHT - 1U; y++)
    {
      drawPixelXY(x, y, getPixColorXY(x, y + 1U));
    }
  }
}
/*
// --------------------------------------
void effectsTick() {
  snowRoutine();

  FastLED.show();
}
*/
