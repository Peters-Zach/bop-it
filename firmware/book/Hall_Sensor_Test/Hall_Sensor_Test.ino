/*
  Two analog Hall sensors on the book's ESP32-S3-DevKitC-1.

  Wiring from the supplied main.cpp:
    Hall sensor 1 analog output -> GPIO1
    Hall sensor 2 analog output -> GPIO2
    Both sensor grounds        -> GND
    For DFRobot DFR1132 modules, both VCC pins -> 3V3.

  Select ESP32S3 Dev Module in Arduino IDE. Upload and monitor through the
  CP210x USB-to-UART connector. Serial Monitor: 115200 baud.

  Move a magnet toward each sensor, then away; also try the opposite pole.
  This test reports raw ADC counts and measured mV, not calibrated gauss.
  No OLED or additional library is required.
*/

#include <Arduino.h>

constexpr uint8_t HALL1_PIN = 1;
constexpr uint8_t HALL2_PIN = 2;
constexpr uint8_t SAMPLES = 16;
constexpr uint32_t UPDATE_INTERVAL_MS = 100;

void setup() {
  // Serial0 keeps the console on the CP210x connector even if USB CDC changes.
  Serial0.begin(115200);
  pinMode(HALL1_PIN, INPUT);
  pinMode(HALL2_PIN, INPUT);
  analogReadResolution(12);
  analogSetPinAttenuation(HALL1_PIN, ADC_11db);
  analogSetPinAttenuation(HALL2_PIN, ADC_11db);
  delay(250);

  Serial0.println("\nBook Hall sensor test: H1 = GPIO1, H2 = GPIO2");
  Serial0.println("Move a magnet toward each sensor and away; try both poles.");
  Serial0.println("Raw: 0-4095. mV: measured voltage. No detection threshold is assumed.");
}

void loop() {
  const uint32_t startedAt = millis();
  uint32_t raw1Total = 0;
  uint32_t raw2Total = 0;
  uint32_t mv1Total = 0;
  uint32_t mv2Total = 0;

  for (uint8_t sample = 0; sample < SAMPLES; ++sample) {
    raw1Total += analogRead(HALL1_PIN);
    raw2Total += analogRead(HALL2_PIN);
    // These calibrated voltage readings use separate ADC conversions.
    mv1Total += analogReadMilliVolts(HALL1_PIN);
    mv2Total += analogReadMilliVolts(HALL2_PIN);
    delayMicroseconds(200);
  }

  Serial0.print("H1_raw:");
  Serial0.print(raw1Total / SAMPLES);
  Serial0.print("\tH2_raw:");
  Serial0.print(raw2Total / SAMPLES);
  Serial0.print("\tH1_mV:");
  Serial0.print(mv1Total / SAMPLES);
  Serial0.print("\tH2_mV:");
  Serial0.println(mv2Total / SAMPLES);

  // Approximately ten reports per second, including the sampling time.
  const uint32_t elapsed = millis() - startedAt;
  if (elapsed < UPDATE_INTERVAL_MS) {
    delay(UPDATE_INTERVAL_MS - elapsed);
  }
}
