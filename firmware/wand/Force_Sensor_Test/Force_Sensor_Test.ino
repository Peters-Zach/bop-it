/*
  Adafruit #166 force-sensitive resistor test on Seeed XIAO ESP32-S3.

  Wiring:
    XIAO 3V3 ---- FSR ----+---- 10 kohm resistor ---- GND
                         |
                      D0 / GPIO1

  The FSR has no polarity. Use 3.3 V for this voltage divider.

  Arduino IDE:
    Board: XIAO_ESP32S3 (under esp32 by Espressif Systems)
    USB CDC On Boot: Enabled
    Port: the XIAO's USB port
    Serial Monitor or Serial Plotter: 115200 baud

  Released: readings should be near zero.
  Pressed: readings should increase.
  Raw is a 12-bit ADC reading (0-4095); mV is measured voltage.
  These values are NOT a calibrated force measurement in newtons.
*/

#include <Arduino.h>

constexpr uint8_t FSR_PIN = 1;         // XIAO D0 / GPIO1.
constexpr uint8_t SAMPLES = 16;        // Average to reduce jitter.
constexpr uint32_t READ_DELAY_MS = 100;

void setup() {
  Serial.begin(115200);

  // Wait briefly for USB Serial, but keep running without a computer attached.
  const uint32_t waitStarted = millis();
  while (!Serial && millis() - waitStarted < 3000) {
    delay(10);
  }

  pinMode(FSR_PIN, INPUT);             // Divider provides the pull-down.
  analogReadResolution(12);
  analogSetPinAttenuation(FSR_PIN, ADC_11db);

  Serial.println("Force sensor test: XIAO ESP32-S3, D0 / GPIO1");
  Serial.println("Press and release the sensing circle. Raw and mV should rise when pressed.");
}

void loop() {
  uint32_t rawTotal = 0;
  uint32_t millivoltTotal = 0;

  for (uint8_t sample = 0; sample < SAMPLES; ++sample) {
    rawTotal += analogRead(FSR_PIN);
    // Calibrated voltage uses its own ADC conversion, not raw * 3300 / 4095.
    millivoltTotal += analogReadMilliVolts(FSR_PIN);
    delayMicroseconds(200);
  }

  const uint32_t raw = rawTotal / SAMPLES;
  const uint32_t millivolts = millivoltTotal / SAMPLES;

  // Labels work in both Serial Monitor and Serial Plotter.
  Serial.print("Raw:");
  Serial.print(raw);
  Serial.print("\tmV:");
  Serial.println(millivolts);

  delay(READ_DELAY_MS);
}
