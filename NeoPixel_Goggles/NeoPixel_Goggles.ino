#include <Adafruit_NeoPixel.h>
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
 #include <avr/power.h>
#endif

#define PIN 0

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(32, PIN);

const float TAU = 2.0 * PI;
const uint8_t NUM_MODES = 2;
const unsigned long MODE_DURATION[NUM_MODES] = { 10000, 15000 }; // times are in ms

uint8_t mode   = 0; // Current animation effect
unsigned long lastModeSwitch = millis();
uint8_t hue = 0;

const uint8_t PROGMEM gamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

void setup() {
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
  if(F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  pixels.begin();
  //pixels.setBrightness(85); // 1/3 brightness
  pixels.show(); // Initialize all pixels to 'off'
}

void loop() {
  unsigned long time = millis();
  if(time - lastModeSwitch > MODE_DURATION[mode]) {
    mode = (mode+1) % NUM_MODES;
    lastModeSwitch = time;
  }

  switch(mode) {

    case 0: // rainbow spinners
      spin(0, 16, 2, 1.5, 32, 255, 0, 0);
      spin(16, 16, 2, 1.5, 32, 255, 1, 0);
      hue = (hue+1) % 255;
      break;

    case 1: // pulse
      oscillate(0, 16, 128, 255, 3.0, 0.0);
      oscillate(16, 16, 128, 255, 3.0, PI);
      hue = (hue+1) % 255;
      break;

  }
  pixels.show();
  delay(10);
}

void oscillate(uint8_t startPixel, uint8_t pixelLength, uint8_t minAlpha, uint8_t maxAlpha, float freq, float phase)
{
  unsigned long time = millis();
  uint8_t alpha = (maxAlpha-minAlpha)*0.5*(sin((time/1000.0)*freq*TAU + phase)+1.0)+minAlpha;
  setPixelRange(startPixel, pixelLength, getColor(hue, alpha));
}

void spin(uint8_t startPixel, uint8_t pixelLength, uint8_t numPeaks, float freq, uint8_t minAlpha, uint8_t maxAlpha, uint8_t dir, float phase) {
  unsigned long time = millis();
  uint8_t alpha;
  uint8_t pos;
  
  for(uint8_t i=0; i<pixelLength; i++)
  {
    alpha = (maxAlpha-minAlpha)*0.5*(1.0+sin(TAU*(numPeaks*(1.0*i/pixelLength)+(time/1000.0)*freq)+phase))+minAlpha;
    if(dir == 0)
    {
      pos = startPixel+i;
    }
    else
    {
      pos = startPixel+pixelLength-i-1;
    }
      pixels.setPixelColor(pos, getColor(hue, alpha));
  }  
}

void setPixelRange(uint8_t startPixel, uint8_t pixelLength, uint32_t c) {
  for(uint8_t i=startPixel; i<(startPixel+pixelLength); i++) {
      pixels.setPixelColor(i, c);
  }
}

uint32_t getColor(uint8_t hue, uint8_t alpha) {
  uint8_t w = hue;
  float brightness = pgm_read_byte(&gamma[alpha]) / 255.0;
  if(w < 85) {
    return Adafruit_NeoPixel::Color(w*3*brightness, (255-w*3)*brightness, 0);
  } else if(w < 170) {
    w -= 85;
    return Adafruit_NeoPixel::Color((255-w*3)*brightness, 0, w*3*brightness);
  } else {
    w -= 170;
    return Adafruit_NeoPixel::Color(0, w*3*brightness, (255-w*3)*brightness);
  }  
}
