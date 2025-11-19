# Slimme weegschaal aansluitschema

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
