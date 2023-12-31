#include <FastLED.h>
#define LED_TYPE    WS2812B
#define NUM_LEDS 100
#define DATA_PIN 6
#define BRIGHTNESS 180
#define maxValue 159
CRGB leds[NUM_LEDS];

int nombreObjets = 6;
int indexObjets = 0;

int objet_hue[6];
int objet_sat[6];
int objet_valeurs[6][106];
int objet_centre[6];
int objet_spread[6];
int objet_interval[6];
int objet_sens[6];
int objet_enveloppe_centre[6];
unsigned long objet_enveloppe_spread[6];
unsigned long objet_prevRefresh[6];
unsigned long objet_prevCreate[6];

int palette[] = {0, 160};
int moduloLoopProg = 210;

float calculGaussPosition(float centre, float spread, float x) {
  return round( maxValue * exp( -(x - centre) * (x - centre) / (spread)) );
}

float enveloppeGauss(float centre, float spread, float x) {
  return exp( -(x - centre) * (x - centre) / (spread));
}

void afficherObjet (int hue, int saturation, int centre, int spread) {
  for (int i = 0; i < NUM_LEDS; i++) {
    addLedHSV(i, hue, saturation, gauss(centre, spread, i));
  }
}

void addLedHSV(int ledPosition, int hue, int saturation, int value) {
  leds[ledPosition] += CHSV(hue, saturation, value);
}

void setLedHSV(int ledPosition, int hue, int saturation, int value) {
  leds[ledPosition] = CHSV(hue, saturation, value);
}

float gauss(float centre, float spread, float x) {
  return round( maxValue * exp( -(x - centre) * (x - centre) / (spread)) );
}

void calculerTableauValeurs (float centre, float spread, int indexTableau) {
  for (int i = 0; i < NUM_LEDS; i++) {
    int temp = centre;
    //objet_valeurs[indexTableau][i] = gauss(centre, spread, i);
    objet_valeurs[indexTableau][i] = gauss(centre, spread, (i + temp) % NUM_LEDS);
  }
}


void creerObjet(int hue, int sat, int centre, int spread, int interval, int sens, int enveloppe_centre, unsigned long enveloppe_spread) {
  objet_hue[indexObjets] = hue;
  objet_sat[indexObjets] = sat;
  objet_centre[indexObjets] = centre;
  objet_spread[indexObjets] = spread;
  objet_interval[indexObjets] = interval;
  objet_sens[indexObjets] = sens;
  objet_prevCreate[indexObjets] = millis();
  objet_enveloppe_centre[indexObjets] = enveloppe_centre;
  objet_enveloppe_spread[indexObjets] = enveloppe_spread;

  calculerTableauValeurs(centre, spread, indexObjets);
  indexObjets = (indexObjets + 1) % nombreObjets;
}

void shiftTableau(int indexObjets) {
  if (millis() - objet_prevRefresh[indexObjets] > objet_interval[indexObjets]) {
    objet_prevRefresh[indexObjets] = millis();
    if (objet_sens[indexObjets] == 1) {
      int temp = objet_valeurs[indexObjets][0];
      for (int i = 0; i < (NUM_LEDS - 1); i++) {
        objet_valeurs[indexObjets][i] = objet_valeurs[indexObjets][(i + 1)];
      }
      objet_valeurs[indexObjets][NUM_LEDS - 1] = temp;
    } else {
      int temp = objet_valeurs[indexObjets][NUM_LEDS - 1];
      for (int i = (NUM_LEDS - 1); i > 0; i--) {
        objet_valeurs[indexObjets][i] = objet_valeurs[indexObjets][(i - 1)];
      }
      objet_valeurs[indexObjets][0] = temp;
    }
  }
}

void setObjet(int indexObjets) {
  float tempEnv = enveloppeGauss(objet_enveloppe_centre[indexObjets], objet_enveloppe_spread[indexObjets], millis() - objet_prevCreate[indexObjets]);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] += CHSV(objet_hue[indexObjets], objet_sat[indexObjets], objet_valeurs[indexObjets][i] * tempEnv);
  }
}

void clr() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
}

void afficherObjets() {
  clr();
  for (int i = 0; i < nombreObjets; i++) {
    shiftTableau(i);
    setObjet(i);
  }
}




void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  //Serial.begin(9600);
}

int palette_centres[] = {0, NUM_LEDS/2,  NUM_LEDS - 1};
int palette_RB[] = {0, 160};
int index_palette_RB = 0;

void loop() {
  //Serial.println(random16());


// gaussiennes
  if (random16() < 375) {
    if (bseconds16()%180 < 30) {
      creerObjet(   160,          // hue
                    255,                  // sat
                    NUM_LEDS/2,         // centreGauss
                    random(50, 1500),     // spreadGauss
                    random(2, 20),        // interval
                    random(2),            // sens
                    random(3000, 6000),       // enveloppe gauss centre
                    random(1000000, 5000000)  // enveloppe gauss spread
                );
      
    } else if (bseconds16()%180 < 90) {
      creerObjet(   palette_RB[index_palette_RB],          // hue
                    255,                  // sat
                    NUM_LEDS/2,         // centreGauss
                    random(50, 1500),     // spreadGauss
                    random(2, 20),        // interval
                    random(2),            // sens
                    random(3000, 6000),       // enveloppe gauss centre
                    random(1000000, 5000000)  // enveloppe gauss spread
                );
      index_palette_RB = (index_palette_RB + 1) % 2;
    } else if (bseconds16()%180 < 120){
      creerObjet(   random(224,287)%255,          // hue
                    255,                  // sat
                    NUM_LEDS/2,         // centreGauss
                    random(50, 1500),     // spreadGauss
                    random(2, 40),        // interval
                    random(2),            // sens
                    random(3000, 6000),       // enveloppe gauss centre
                    random(1000000, 5000000)  // enveloppe gauss spread
                );
    } else if (bseconds16()%180 < 180){
      creerObjet(   random(255),          // hue
                    255,                  // sat
                    NUM_LEDS/2,         // centreGauss
                    random(50, 1500),     // spreadGauss
                    random(2, 40),        // interval
                    random(2),            // sens
                    random(3000, 6000),       // enveloppe gauss centre
                    random(1000000, 5000000)  // enveloppe gauss spread
                );
    } 
  }


// filantes
// mauvais cas (intensité max au cul ==> centre 0 sens 0
//  if (random16() < 375) {
//      if (bseconds16()%180 < 30) {
//        creerObjet(   palette_RB[index_palette_RB],          // hue
//                      255,                  // sat
//                      0,         // centreGauss
//                      random(1000, 1500),     // spreadGauss
//                      random(2, 5),        // interval
//                      1,            // sens
//                      random(3000, 6000),       // enveloppe gauss centre
//                      random(1000000, 5000000)  // enveloppe gauss spread
//                  );
//        index_palette_RB = (index_palette_RB + 1) % 2;
//      }
//  }




  
  afficherObjets();
  FastLED.show();
}


//  if (random16() < 375) {
//    if (bseconds16()%180 < 30) {
//      creerObjet(   160,          // hue
//                    255,                  // sat
//                    NUM_LEDS / 2,         // centreGauss
//                    random(50, 1500),     // spreadGauss
//                    random(2, 20),        // interval
//                    random(2),            // sens
//                    random(3000, 6000),       // enveloppe gauss centre
//                    random(1000000, 5000000)  // enveloppe gauss spread
//                );
//    } else if (bseconds16()%180 < 90) {
//      creerObjet(   palette[random(2)],          // hue
//                    255,                  // sat
//                    NUM_LEDS / 2,         // centreGauss
//                    random(50, 1500),     // spreadGauss
//                    random(2, 20),        // interval
//                    random(2),            // sens
//                    random(3000, 6000),       // enveloppe gauss centre
//                    random(1000000, 5000000)  // enveloppe gauss spread
//                );
//    } else if (bseconds16()%180 < 120){
//      creerObjet(   random(224,287)%255,          // hue
//                    255,                  // sat
//                    NUM_LEDS / 2,         // centreGauss
//                    random(50, 1500),     // spreadGauss
//                    random(2, 40),        // interval
//                    random(2),            // sens
//                    random(3000, 6000),       // enveloppe gauss centre
//                    random(1000000, 5000000)  // enveloppe gauss spread
//                );
//    } else if (bseconds16()%180 < 180){
//      creerObjet(   random(255),          // hue
//                    255,                  // sat
//                    NUM_LEDS / 2,         // centreGauss
//                    random(50, 1500),     // spreadGauss
//                    random(2, 40),        // interval
//                    random(2),            // sens
//                    random(3000, 6000),       // enveloppe gauss centre
//                    random(1000000, 5000000)  // enveloppe gauss spread
//                );
//    } 
//  }
