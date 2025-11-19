#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HX711.h>
#include <ESP32Encoder.h>
#include <FastLED.h>

// Pin definities volgens aansluitschema
#define HX711_DOUT  5
#define HX711_SCK   18
#define ENCODER_CLK 19
#define ENCODER_DT  32
#define ENCODER_SW  33
#define I2C_SDA     21
#define I2C_SCL     22
#define LED_PIN     23
#define NUM_LEDS    10

// OLED Display instellingen
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// Applicatie states
enum AppMode {
  WEIGHING_MODE,
  SETTING_MODE
};

// Globals
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
HX711 scale;
ESP32Encoder encoder;
CRGB leds[NUM_LEDS];

AppMode currentMode = WEIGHING_MODE;
float calibrationFactor = 393;  // Gekalibreerd gewicht
int targetWeight = 10;  // Streefgewicht in gram (start bij 10)
float currentWeight = 0.0;

// Button debounce
unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 200;
bool lastButtonState = HIGH;

// Scale reading timing
unsigned long lastScaleRead = 0;
const unsigned long scaleReadInterval = 150;  // Lees scale elke 150ms

// Previous values voor change detection
int lastDisplayedWeight = -999;
int lastDisplayedTarget = -999;
int lastLEDWeight = -999;

void setup() {
  Serial.begin(115200);
  Serial.println("=== Smart Weegschaal ===");

  // I2C initialiseren
  Wire.begin(I2C_SDA, I2C_SCL);

  // OLED initialiseren
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("FOUT: OLED niet gevonden!");
    for(;;);
  }

  // Encoder initialiseren
  ESP32Encoder::useInternalWeakPullResistors = UP;
  encoder.attachHalfQuad(ENCODER_DT, ENCODER_CLK);
  encoder.setCount(targetWeight);  // Start positie

  pinMode(ENCODER_SW, INPUT_PULLUP);

  // LED strip initialiseren
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);  // 50/255 helderheid (pas aan naar wens)
  FastLED.clear();
  FastLED.show();

  // HX711 initialiseren
  scale.begin(HX711_DOUT, HX711_SCK);

  // Startscherm: instructie voor tarra
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println("Smart Weegschaal");
  display.println();
  display.setTextSize(2);
  display.println("Maak");
  display.println("leeg!");
  display.display();

  Serial.println("Wacht 3 seconden voor tarra...");

  // Regenboog animatie tijdens 3 seconden wachttijd
  unsigned long startTime = millis();
  while (millis() - startTime < 3000) {
    // Bereken hue (0-255) op basis van tijd
    uint8_t hue = map(millis() - startTime, 0, 3000, 0, 255);

    // Zet alle LEDs op dezelfde kleur
    fill_solid(leds, NUM_LEDS, CHSV(hue, 255, 255));
    FastLED.show();

    delay(10);  // Kleine delay voor vloeiende animatie
  }

  // Tarra uitvoeren
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.println("Tarra bezig...");
  display.display();

  scale.tare();
  scale.set_scale(calibrationFactor);

  Serial.println("Tarra compleet!");

  // Bevestiging
  display.clearDisplay();
  display.setCursor(0, 20);
  display.println("Gereed!");
  display.display();
  delay(1000);

  Serial.println("Systeem klaar!");
  Serial.println("Klik encoder: wissel modus");
  Serial.println("Draai encoder: instel gewicht");
}

void loop() {
  static AppMode lastMode = WEIGHING_MODE;
  unsigned long currentTime = millis();

  // Button check (non-blocking, met state change detectie)
  bool currentButtonState = digitalRead(ENCODER_SW);

  if (currentButtonState == LOW && lastButtonState == HIGH) {
    // Button net ingedrukt (falling edge)
    if (currentTime - lastButtonPress > debounceDelay) {
      lastButtonPress = currentTime;

      // Wissel mode
      if (currentMode == WEIGHING_MODE) {
        currentMode = SETTING_MODE;
        encoder.setCount(targetWeight);  // Sync encoder met huidig streefgewicht
        Serial.println("Mode: Gewicht instellen");

        // Force redraw
        lastDisplayedWeight = -999;
        lastDisplayedTarget = -999;
      } else {
        currentMode = WEIGHING_MODE;
        Serial.print("Mode: Wegen | Streefgewicht: ");
        Serial.print(targetWeight);
        Serial.println(" gram");

        // Force redraw
        lastDisplayedWeight = -999;
        lastDisplayedTarget = -999;
      }
    }
  }
  lastButtonState = currentButtonState;

  // Scale reading op interval (alleen in weeg mode)
  if (currentMode == WEIGHING_MODE && currentTime - lastScaleRead >= scaleReadInterval) {
    lastScaleRead = currentTime;

    if (scale.is_ready()) {
      currentWeight = scale.get_units(1);  // Snelle single read
    }
  }

  // Display update (alleen als waarde veranderd is)
  if (currentMode == WEIGHING_MODE) {
    // WEEG MODUS
    int displayWeight = (int)currentWeight;

    // Update alleen als waarde veranderd is of mode gewisseld
    if (displayWeight != lastDisplayedWeight || targetWeight != lastDisplayedTarget || lastMode != currentMode) {
      lastDisplayedWeight = displayWeight;
      lastDisplayedTarget = targetWeight;

      display.clearDisplay();

      // Huidig gewicht (integer + gram op 1 regel)
      display.setTextSize(3);
      display.setCursor(0, 0);
      display.print(displayWeight);
      display.print("g");

      // Streefgewicht (onderaan, groter)
      display.setTextSize(2);
      display.setCursor(0, 48);
      display.print(">");
      display.print(targetWeight);
      display.print("g");

      display.display();

      // Serial output
      Serial.print("Gewicht: ");
      Serial.print(displayWeight);
      Serial.print(" g | Doel: ");
      Serial.print(targetWeight);
      Serial.println(" g");
    }

  } else {
    // INSTEL MODUS
    int newTargetWeight = encoder.getCount();

    // Beperk tussen 1 en 999 gram
    if (newTargetWeight < 1) {
      newTargetWeight = 1;
      encoder.setCount(1);
    }
    if (newTargetWeight > 999) {
      newTargetWeight = 999;
      encoder.setCount(999);
    }

    // Update alleen als gewicht veranderd is of mode gewijzigd
    if (newTargetWeight != targetWeight || lastMode != currentMode) {
      targetWeight = newTargetWeight;

      display.clearDisplay();

      // Titel
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println("INSTELLEN");

      // Streefgewicht (extra groot, gecentreerd)
      display.setTextSize(3);
      display.setCursor(15, 20);
      display.print(targetWeight);

      display.setTextSize(2);
      display.setCursor(15, 45);
      display.println("gram");

      display.display();

      // Serial output
      Serial.print("Streefgewicht: ");
      Serial.print(targetWeight);
      Serial.println(" gram");
    }
  }

  lastMode = currentMode;

  // LED update (alleen in weeg mode en als gewicht veranderd is)
  if (currentMode == WEIGHING_MODE) {
    int displayWeight = (int)currentWeight;

    if (displayWeight != lastLEDWeight) {
      lastLEDWeight = displayWeight;

      // Bereken percentage van streefgewicht
      // Bij 0% = 0 LEDs, bij 100% = 10 LEDs groen
      // Bij >100% = LEDs worden rood (1 LED rood per 10% overschrijding)

      float percentage = 0.0;
      if (targetWeight > 0) {
        percentage = (float)displayWeight / (float)targetWeight * 100.0;
      }

      // Aantal LEDs aan (0-10)
      int numLEDsOn = constrain((int)(percentage / 10.0), 0, 10);

      // Clear alle LEDs
      FastLED.clear();

      if (percentage <= 100.0) {
        // Onder of op streefgewicht: groene LEDs
        for (int i = 0; i < numLEDsOn; i++) {
          leds[i] = CRGB::Green;
        }
      } else {
        // Boven streefgewicht: eerst groen tot streefgewicht, dan rood
        // Alle 10 LEDs groen tot 100%
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CRGB::Green;
        }

        // Bereken hoeveel LEDs rood moeten worden (overschrijding)
        float overshootPercentage = percentage - 100.0;
        int numRedLEDs = constrain((int)(overshootPercentage / 10.0), 0, 10);

        // Maak eerste N LEDs rood (van links naar rechts)
        for (int i = 0; i < numRedLEDs; i++) {
          leds[i] = CRGB::Red;
        }
      }

      FastLED.show();
    }
  } else {
    // In instel mode: alle LEDs uit
    if (lastMode != currentMode) {
      FastLED.clear();
      FastLED.show();
    }
  }

  // Geen delay - loop draait zo snel mogelijk voor responsive button
}
