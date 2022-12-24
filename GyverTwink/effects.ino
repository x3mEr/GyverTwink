void effectsTick()
{
  byte thisEffect;
  thisEffect = curEff;
  //!if (forceTmr.state()) thisEffect = forceEff;
  //!else thisEffect = curEff;
    
  switch (thisEffect)
      {
        case EFF_SPARKLES:       sparklesRoutine();           break;
        case EFF_RAINBOW_VER:    rainbowVerticalRoutine();    break;
        case EFF_RAINBOW_HOR:    rainbowHorizontalRoutine();  break;
        case EFF_RAINBOW_DIAG:   rainbowDiagonalRoutine();    break;
        case EFF_COLOR:          colorRoutine();              break;
        case EFF_COLORS:         colorsRoutine();             break;
        case EFF_FIRE:           fireRoutine(true);           break;
        case EFF_WHITE_FIRE:     fireRoutine(false);          break;
        
/*        case EFF_MADNESS:        madnessNoiseRoutine();       break;
        case EFF_CLOUDS:         cloudsNoiseRoutine();        break;
        case EFF_LAVA:           lavaNoiseRoutine();          break;
        case EFF_PLASMA:         plasmaNoiseRoutine();        break;
        case EFF_RAINBOW:        rainbowNoiseRoutine();       break;
        case EFF_RAINBOW_STRIPE: rainbowStripeNoiseRoutine(); break;
        case EFF_ZEBRA:          zebraNoiseRoutine();         break;
        case EFF_FOREST:         forestNoiseRoutine();        break;
        case EFF_OCEAN:          oceanNoiseRoutine();         break;
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
    thisEffect = curEff;

    //!if (forceTmr.state()) thisEffect = forceEff;
    //!else thisEffect = curEff;

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

// ------------- конфетти --------------
#define FADE_OUT_SPEED        (70U)                         // скорость затухания
void sparklesRoutine()
{
  for (int32_t i = 0; i < effs[EFF_SPARKLES].scale / 32.0F; i++)
  {
    
    int32_t thisPixel = random(0, cfg.strAm * cfg.ledAm);
    if (getPixColor(thisPixel) == 0U)
    {
      leds[thisPixel] = CHSV(random(0U, 255U), 255U, 255U);
    }
  }
  fader( (uint8_t)(effs[EFF_SPARKLES].speed / 4.0F) );
}

// ------------- радуга вертикальная ----------------
uint8_t hue;
void rainbowVerticalRoutine()
{
  /*hue += *//*4*/ /*(uint8_t)((effs[EFF_RAINBOW_VER].speed - 128)/16.0F);*/
  hue += /*4*/ (uint8_t)((effs[EFF_RAINBOW_VER].speed)/16.0F);
  for (uint8_t j = mm.minY; j < mm.maxY; j++)
  {
    CHSV thisColor = CHSV((uint8_t)(hue + j * effs[EFF_RAINBOW_VER].scale / 32), 255, 255);
    for (uint8_t i = mm.minX; i < mm.maxX; i++)
    {
      drawPixelXY(i, j, thisColor);
    }
  }
}
// ------------- радуга горизонтальная ----------------
void rainbowHorizontalRoutine()
{
  //hue += (uint8_t)((effs[EFF_RAINBOW_HOR].speed - 128)/16.0F);
  hue += /*4*/(uint8_t)((effs[EFF_RAINBOW_HOR].speed)/16.0F);
  for (uint8_t i = mm.minX; i < mm.maxX; i++)
  {
    CHSV thisColor = CHSV((uint8_t)(hue + i * effs[EFF_RAINBOW_HOR].scale / 32), 255, 255);
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

  //hue += (uint8_t)((effs[EFF_RAINBOW_DIAG].speed - 128)/8.0F);
  hue += /*8*/ (uint8_t)((effs[EFF_RAINBOW_DIAG].speed)/8.0F);
  for (uint8_t i = mm.minX; i < mm.maxX; i++)
  {
    for (uint8_t j = mm.minY; j < mm.maxY; j++)
    {
      float twirlFactor = 3.0F * (effs[EFF_RAINBOW_DIAG].scale / 255.0F);      // на сколько оборотов будет закручена матрица, [0..3]
      CRGB thisColor = CHSV((uint8_t)(hue + (float)(mm.w / mm.h * i + j * twirlFactor) * (float)(255 / max(mm.w, mm.h))), 255, 255);
      drawPixelXY(i, j, thisColor);
    }
  }
}
// ------------- цвета -----------------
void colorsRoutine()
{
  if (loadingFlag)
  {
    hue += effs[EFF_COLORS].scale / 100.0F;

    for (uint16_t i = 0U; i < cfg.strAm * cfg.ledAm; i++)
    {
      leds[i] = CHSV(hue, 255U, 255U);
    }
  }
}

// ------------- цвет ------------------
void colorRoutine()
{
  if (loadingFlag)
  {
    loadingFlag = false;
    FastLED.clear();

    for (int16_t i = 0U; i < cfg.strAm * cfg.ledAm; i++)
    {
      leds[i] = CHSV(effs[EFF_COLOR].scale, 255U, 255U);
    }
  }
}

// ------------- огонь -----------------
#define SPARKLES 1                          // вылетающие угольки вкл выкл
uint8_t line[256];
uint8_t pcnt = 0U;

//these values are substracetd from the generated values to give a shape to the animation
static const uint8_t valueMask[8][16] PROGMEM =
{
  {32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 , 32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 },
  {64 , 0  , 0  , 0  , 0  , 0  , 0  , 64 , 64 , 0  , 0  , 0  , 0  , 0  , 0  , 64 },
  {96 , 32 , 0  , 0  , 0  , 0  , 32 , 96 , 96 , 32 , 0  , 0  , 0  , 0  , 32 , 96 },
  {128, 64 , 32 , 0  , 0  , 32 , 64 , 128, 128, 64 , 32 , 0  , 0  , 32 , 64 , 128},
  {160, 96 , 64 , 32 , 32 , 64 , 96 , 160, 160, 96 , 64 , 32 , 32 , 64 , 96 , 160},
  {192, 128, 96 , 64 , 64 , 96 , 128, 192, 192, 128, 96 , 64 , 64 , 96 , 128, 192},
  {255, 160, 128, 96 , 96 , 128, 160, 255, 255, 160, 128, 96 , 96 , 128, 160, 255},
  {255, 192, 160, 128, 128, 160, 192, 255, 255, 192, 160, 128, 128, 160, 192, 255}
};

//these are the hues for the fire,
//should be between 0 (red) to about 25 (yellow)
static const uint8_t hueMask[8][16] PROGMEM =
{
  {1 , 11, 19, 25, 25, 22, 11, 1 , 1 , 11, 19, 25, 25, 22, 11, 1 },
  {1 , 8 , 13, 19, 25, 19, 8 , 1 , 1 , 8 , 13, 19, 25, 19, 8 , 1 },
  {1 , 8 , 13, 16, 19, 16, 8 , 1 , 1 , 8 , 13, 16, 19, 16, 8 , 1 },
  {1 , 5 , 11, 13, 13, 13, 5 , 1 , 1 , 5 , 11, 13, 13, 13, 5 , 1 },
  {1 , 5 , 11, 11, 11, 11, 5 , 1 , 1 , 5 , 11, 11, 11, 11, 5 , 1 },
  {0 , 1 , 5 , 8 , 8 , 5 , 1 , 0 , 0 , 1 , 5 , 8 , 8 , 5 , 1 , 0 },
  {0 , 0 , 1 , 5 , 5 , 1 , 0 , 0 , 0 , 0 , 1 , 5 , 5 , 1 , 0 , 0 },
  {0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 }
};

void fireRoutine(bool isColored)                            // true - цветной огонь, false - белый
{
  if (loadingFlag)
  {
    loadingFlag = false;
    FastLED.clear();
    generateLine();
    memset(matrixValue, 0, sizeof(matrixValue));
  }
  if (pcnt >= 100)
  {
    shiftUp();
    generateLine();
    pcnt = 0;
  }
  drawFrame(pcnt, isColored);
  pcnt += 30;
}

// Randomly generate the next line (matrix row)
void generateLine()
{
  for (uint8_t x = 0; x < 128; x++)
  {
    line[x] = random(64, 255);
  }
}

void shiftUp()
{
  for (uint8_t y = 16 - 1U; y > 0; y--)
  {
    for (uint8_t x = 0; x < 128; x++)
    {
      uint8_t newX = x;
      if (x > 15U) newX = x % 16U;
      if (y > 7U) continue;
      matrixValue[y][newX] = matrixValue[y - 1U][newX];
    }
  }

  for (uint8_t x = 0; x < 128; x++)
  {
    uint8_t newX = x;
    if (x > 15U) newX = x % 16U;
    matrixValue[0U][newX] = line[newX];
  }
}

// draw a frame, interpolating between 2 "key frames"
// @param pcnt percentage of interpolation

void drawFrame(uint8_t pcnt, bool isColored)
{
  int32_t nextv;
  int dx=0,dy=effs[EFF_FIRE].speed;

  //each row interpolates with the one before it
  for (uint8_t y = 16 - 1U; y > 0; y--)
  {
    for (uint8_t x = 0; x < 128; x++)
    {
      uint8_t newX = x;
      if (x > 15U) newX = x % 16U;
      if (y < 8U)
      {
        nextv =
          (((100.0 - pcnt) * matrixValue[y][newX]
            + pcnt * matrixValue[(y - 1)][newX]) / 100.0)
          - pgm_read_byte(&valueMask[y][newX]);

        CRGB color = CHSV(
          isColored ? effs[EFF_FIRE].scale * 2.5 / 255.0 + pgm_read_byte(&hueMask[y][newX]) : 0U,     // H
          isColored ? 255U : 0U,                                                               // S
          (uint8_t)max(0, nextv)                                                               // V
        );

        leds[getPixelNumber(dx+x, dy+y)] = color;
      }
      else if (y == 8U && SPARKLES)
      {
        if (random(0, 20) == 0 && getPixColorXY(dx+x, dy+y - 1U) != 0U) drawPixelXYD(dx+x, dy+y, 5, getPixColorXY(dx+x, dy+y - 1U));
        else drawPixelXYD(dx+x, dy+y, 5, 0U);
      }
      else if (SPARKLES)
      {
        // старая версия для яркости
        if (getPixColorXY(dx+x, dy+y - 1U) > 0U)
          drawPixelXYD(dx+x, dy+y, 5, getPixColorXY(dx+x, dy+y - 1U));
        else drawPixelXYD(dx+x, dy+y, 5, 0U);
      }
    }
  }

  //first row interpolates with the "next" line
  for (uint8_t x = 0; x < 128; x++)
  {
    uint8_t newX = x;
    if (x > 15U) newX = x % 16U;
    CRGB color = CHSV(
      isColored ? effs[EFF_FIRE].scale * 2.5 / 255.0 + pgm_read_byte(&(hueMask[0][newX])): 0U,        // H
      isColored ? 255U : 0U,                                                                   // S
      (uint8_t)(((100.0 - pcnt) * matrixValue[0][newX] + pcnt * line[newX]) / 100.0)           // V
    );
    //leds[getPixelNumber(newX, 0)] = color;                                         // на форуме пишут что это ошибка - вместо newX должно быть x, иначе
    leds[getPixelNumber(dx+x, 0)] = color;                                              // на матрицах шире 16 столбцов нижний правый угол неработает
  }
}

void fire2021() {
    byte scale = 64; // 0..255
    byte speed = 20; // 0..255
    t += speed;
    for (int i = 0; i < cfg.strAm * cfg.ledAm; i++) {
      int16_t Bri = inoise8(xy[i][0] * scale, (xy[i][1] * scale) - t) - (xy[i][1] * HEIGHTCORRECTOR);
      byte Col = Bri;
      if (Bri <= 0) 
        Bri = 0; 
      else
        Bri = 256 - (Bri * 0.2);
      nblend(emuleds[i], ColorFromPalette(firePaletteArr[1], Col, Bri), speed);
    }
}
void fire2021Routine() {
  if (loadingFlag) {
    loadingFlag = false;
    for (int i = 0; i < cfg.strAm * cfg.ledAm; i++) {
      uint16_t temp = random16((N_LEDS+COLS+ROWS)/2);
      //xy[i][0] = random8(COLS);
      //xy[i][1] = random8(ROWS);
      uint8_t tmpX = COLS;
      uint8_t tmpY = 0U;
      while (temp >= tmpX){
        temp -= tmpX;
        tmpX--;
        tmpY++;
      }
      xy[i][0] = temp+(COLS-tmpX)/2;
      xy[i][1] = tmpY;
    }
  }
  fire2021();
  for (int i = 0; i < cfg.strAm * cfg.ledAm; i++) {
    if (uint8_t(xy[i][0]) < COLS && uint8_t(xy[i][1]) < ROWS){
      leds[ledIdxByXY(xy[i][0],xy[i][1],2)] = emuleds[i];
    }
  }
}



// ------------------------------------
// ------------------------------------
// ------------------------------------
// ------------------------------------
// ------------------------------------
// ------------------------------------
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
