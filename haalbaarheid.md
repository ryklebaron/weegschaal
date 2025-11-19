# Haalbaarheid Smart Weegschaal Project

## Haalbaarheid

**Met ESP8266 (huidige board): NIET AANBEVOLEN**
- ESP8266 heeft slechts 1 echte ADC pin - probleem voor loadcell
- Beperkte GPIO pins (11 bruikbare)
- Wel voldoende voor dit project als je I2C gebruikt, maar krap

**Met ESP32: VOLLEDIG HAALBAAR ✓**
- Ruim voldoende GPIO pins (tot 34)
- Meerdere ADC kanalen beschikbaar
- Betere performance en stabieliteit

## Componenten & Aansluitingen

| Component | Interface | ESP32 Pins |
|-----------|-----------|------------|
| Load cell + HX711 driver | Digitaal (2 pins) | DOUT + SCK |
| Rotary encoder | Digitaal (3 pins) | CLK, DT, SW |
| OLED display | I2C (2 pins) | SDA (GPIO21), SCL (GPIO22) |
| WS2812B strip | Digitaal (1 pin) | Elke GPIO (bijv. GPIO23) |

**Totaal: 8 pins nodig** - geen probleem voor ESP32!

## Voor- en Nadelen

**Voordelen:**
- Geen ADC nodig (HX711 driver regelt dit)
- I2C bespaart pins (OLED)
- WS2812B gebruikt slechts 1 pin voor 10+ LEDs
- Visueel concept is toegankelijk en intuïtief

**Nadelen:**
- HX711 calibratie vereist precisie
- Load cell gevoelig voor temperatuur/trillingen
- 3 gram precisie bij overschrijding is uitdagend

## Kosten (indicatief)

- ESP32 dev board: €5-8
- HX711 + load cell (5kg): €3-5
- Rotary encoder: €1-2
- OLED display 0.96" I2C: €3-4
- WS2812B strip (10 LEDs): €2-3
- Diverse (kabels, PCB, voeding): €3-5

**Totaal: €17-27**

## Load Cell Bedrading (4 draden)

**Standaard kleuren naar HX711:**

| Load Cell Kleur | Functie | HX711 Aansluiting |
|-----------------|---------|-------------------|
| **Rood** | Excitation + (VCC) | E+ |
| **Zwart** | Excitation - (GND) | E- |
| **Wit** | Signal + (Output) | A+ |
| **Groen** | Signal - (Output) | A- |

**Let op:** Als de load cell niet goed werkt, verwissel dan wit en groen (A+ en A-). Dit geeft alleen een omgekeerde waarde.

## Compleet Aansluitschema

```
Load Cell          HX711          ESP32
─────────────────────────────────────────────────
Rood    ────────→  E+
Zwart   ────────→  E-
Wit     ────────→  A+
Groen   ────────→  A-
                   VCC  ────────→ 5V (of 3.3V)
                   GND  ────────→ GND
                   DOUT ────────→ GPIO5
                   SCK  ────────→ GPIO18

Rotary Encoder                   ESP32
─────────────────────────────────────────────────
                   CLK  ────────→ GPIO19
                   DT   ────────→ GPIO32
                   SW   ────────→ GPIO33
                   VCC  ────────→ 3.3V
                   GND  ────────→ GND

OLED Display                     ESP32
─────────────────────────────────────────────────
                   SDA  ────────→ GPIO21
                   SCL  ────────→ GPIO22
                   VCC  ────────→ 3.3V
                   GND  ────────→ GND

WS2812B Strip                    ESP32
─────────────────────────────────────────────────
                   DIN  ────────→ GPIO23
                   VCC  ────────→ 5V
                   GND  ────────→ GND
```

## Conclusie

**Haalbaar met ESP32.** ESP8266 is te beperkt. Upgrade naar ESP32 aanbevolen (€3 meerprijs).

### Aanbeveling

Wijzig `platformio.ini` naar:
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
```
