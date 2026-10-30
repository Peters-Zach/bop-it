/*
  Wand: XIAO ESP32-S3 + Adafruit DRV2605L + ERM vibration motor (#1201).
  A 500 ms buzz starts every 10 seconds, without Serial Monitor commands.
  The first buzz starts 10 seconds after successful driver initialization.

  DRV2605L VIN -> 3V3          GND -> GND
           SDA -> D4 / GPIO5   SCL -> D5 / GPIO6
        IN/INT -> GND (unused trigger input)
  Motor red -> driver motor +; motor blue -> driver motor -.
  Neither motor wire connects directly to a GPIO or GND.

  Board: XIAO_ESP32S3. USB CDC On Boot: Enabled.
  Serial Monitor: 115200 baud via the XIAO USB-C connector.
  Library: Adafruit DRV2605 Library (and Adafruit BusIO).
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_DRV2605.h>

#if !defined(ARDUINO_USB_CDC_ON_BOOT) || !ARDUINO_USB_CDC_ON_BOOT
#error "Enable Tools > USB CDC On Boot, then compile/upload again."
#endif

constexpr int SDA_PIN = 5;  // XIAO D4, not GPIO4.
constexpr int SCL_PIN = 6;  // XIAO D5.
constexpr uint32_t VIBRATION_INTERVAL_MS = 10000;  // Start-to-start interval.
constexpr uint32_t VIBRATION_DURATION_MS = 500;
constexpr uint32_t RETRY_INTERVAL_MS = 2000;
constexpr uint8_t VIBRATION_STRENGTH = 127;  // Signed RTP: 0=off, 127=full scale.

// DRV2605L datasheet section 8.5.2.1, ERM open-loop equation:
// full-scale average drive = 0.02159 V * OD_CLAMP; 138 gives about 2.98 V.
// VIN must still be 3.3V for this breakout's I2C pull-ups and our coin motor.
constexpr uint8_t MOTOR_CLAMP_CODE = 138;

static_assert(VIBRATION_DURATION_MS > 0 &&
              VIBRATION_DURATION_MS < VIBRATION_INTERVAL_MS,
              "Choose a positive buzz duration shorter than the interval.");
static_assert(VIBRATION_STRENGTH > 0 && VIBRATION_STRENGTH <= 127,
              "Signed RTP strength must be 1-127.");

Adafruit_DRV2605 driver;
bool driverReady = false;
bool vibrating = false;
uint32_t lastBuzzStartedAt = 0;
uint32_t lastRetryAt = 0;

bool readDriverRegister(uint8_t reg, uint8_t &value) {
  Wire.beginTransmission(DRV2605_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(DRV2605_ADDR, static_cast<uint8_t>(1)) != 1) return false;
  value = Wire.read();
  return true;
}

bool initializeDriver() {
  vibrating = false;
  if (!driver.begin(&Wire)) {
    Serial.println("DRV2605L not found at 0x5A. Retrying in 2 seconds.");
    Serial.println("Check VIN->3V3, GND, SDA->D4/GPIO5, SCL->D5/GPIO6.");
    return false;
  }

  driver.setRealtimeValue(0);
  driver.setMode(DRV2605_MODE_INTTRIG);  // Idle until the timer starts a buzz.
  driver.stop();                       // Clear any old waveform GO command.
  driver.useERM();

  uint8_t control3 = 0;
  if (!readDriverRegister(DRV2605_REG_CONTROL3, control3)) return false;
  // ERM open-loop=1, supply compensation enabled, signed RTP input.
  control3 = (control3 | 0x20) & ~0x18;
  driver.writeRegister8(DRV2605_REG_CONTROL3, control3);
  driver.writeRegister8(DRV2605_REG_CLAMPV, MOTOR_CLAMP_CODE);

  // The library's write calls do not report errors, so verify our settings.
  uint8_t actualControl3 = 0;
  uint8_t actualClamp = 0;
  uint8_t actualFeedback = 0;
  if (!readDriverRegister(DRV2605_REG_CONTROL3, actualControl3) ||
      !readDriverRegister(DRV2605_REG_CLAMPV, actualClamp) ||
      !readDriverRegister(DRV2605_REG_FEEDBACK, actualFeedback) ||
      actualControl3 != control3 || actualClamp != MOTOR_CLAMP_CODE ||
      (actualFeedback & 0x80) != 0) {
    Serial.println("Could not verify driver settings. Retrying in 2 seconds.");
    return false;
  }

  lastBuzzStartedAt = millis();
  Serial.println("DRV2605L ready: ERM motor, approximately 3V full-scale drive.");
  Serial.println("500 ms buzz every 10 seconds. First buzz in 10 seconds.");
  return true;
}

void setup() {
  Serial.begin(115200);
  const uint32_t waitStartedAt = millis();
  while (!Serial && millis() - waitStartedAt < 3000) delay(10);

  Serial.println("\nWand - DRV2605L haptic test");
  Wire.begin(SDA_PIN, SCL_PIN, 100000);
  Wire.setTimeOut(50);
  driverReady = initializeDriver();
  lastRetryAt = millis();
}

void loop() {
  const uint32_t now = millis();
  if (!driverReady) {
    if (now - lastRetryAt >= RETRY_INTERVAL_MS) {
      lastRetryAt = now;
      driverReady = initializeDriver();
    }
    delay(1);
    return;
  }

  if (vibrating) {
    if (now - lastBuzzStartedAt >= VIBRATION_DURATION_MS) {
      // stop() alone only clears GO; RTP must be zeroed or its mode exited.
      driver.setRealtimeValue(0);
      driver.setMode(DRV2605_MODE_INTTRIG);
      vibrating = false;
      Serial.println("Vibration OFF");
    }
  } else if (now - lastBuzzStartedAt >= VIBRATION_INTERVAL_MS) {
    Wire.beginTransmission(DRV2605_ADDR);
    if (Wire.endTransmission() != 0) {
      Serial.println("Lost contact with the driver. Retrying in 2 seconds.");
      driverReady = false;
      lastRetryAt = millis();
    } else {
      driver.setRealtimeValue(0);
      driver.setMode(DRV2605_MODE_REALTIME);
      driver.setRealtimeValue(VIBRATION_STRENGTH);
      lastBuzzStartedAt = millis();
      vibrating = true;
      Serial.println("Vibration ON (500 ms)");
    }
  }
  delay(1);
}
