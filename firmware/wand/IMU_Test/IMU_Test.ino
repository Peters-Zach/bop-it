/*
  Adafruit LSM6DSOX IMU test on Seeed XIAO ESP32-S3.

  IMU VIN -> XIAO 3V3      IMU GND -> XIAO GND
  IMU SDA -> XIAO D4/GPIO5 IMU SCL -> XIAO D5/GPIO6
  No interrupt or extra GPIO connection is needed.

  Arduino IDE: XIAO_ESP32S3, USB CDC On Boot = Enabled.
  ESP32S3 Dev Module also works with USB CDC On Boot = Enabled.
  Select the XIAO's USB port, upload, then open Serial Monitor at 115200.

  Library: Adafruit LSM6DS, including its BusIO and Unified Sensor dependencies.
  Output: acceleration in m/s^2; rotation rate in degrees/second.
  At rest, acceleration magnitude is about 9.8 m/s^2 (gravity), not zero.
  Gyroscope readings should be close to zero while stationary.
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_LSM6DSOX.h>

#if !defined(ARDUINO_USB_CDC_ON_BOOT) || !ARDUINO_USB_CDC_ON_BOOT
#error "Enable Tools > USB CDC On Boot, then compile/upload again."
#endif

constexpr int SDA_PIN = 5;  // XIAO D4, not GPIO4.
constexpr int SCL_PIN = 6;  // XIAO D5.
constexpr uint8_t WHO_AM_I_REGISTER = 0x0F;
constexpr uint8_t LSM6DSOX_ID = 0x6C;

Adafruit_LSM6DSOX imu;
uint8_t imuAddress = 0;
bool imuReady = false;

bool hasExpectedIdentity(uint8_t address) {
  Wire.beginTransmission(address);
  Wire.write(WHO_AM_I_REGISTER);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(address, static_cast<uint8_t>(1)) != 1) return false;
  return Wire.read() == LSM6DSOX_ID;
}

bool initializeImu() {
  const uint8_t addresses[] = {0x6A, 0x6B};

  for (uint8_t address : addresses) {
    if (hasExpectedIdentity(address) && imu.begin_I2C(address, &Wire)) {
      imuAddress = address;
      imu.setAccelRange(LSM6DS_ACCEL_RANGE_4_G);
      imu.setGyroRange(LSM6DS_GYRO_RANGE_500_DPS);
      imu.setAccelDataRate(LSM6DS_RATE_104_HZ);
      imu.setGyroDataRate(LSM6DS_RATE_104_HZ);
      delay(100);  // Allow the first measurements to become available.

      Serial.print("LSM6DSOX found at 0x");
      Serial.println(imuAddress, HEX);
      Serial.println("Acceleration: m/s^2 | Gyroscope: deg/s");
      Serial.println("Tilt the board, then rotate it to test all six axes.");
      return true;
    }
  }

  Serial.println("LSM6DSOX not found at 0x6A or 0x6B. Retrying in 2 seconds.");
  Serial.println("Check VIN->3V3, GND->GND, SDA->D4/GPIO5, SCL->D5/GPIO6.");
  return false;
}

void setup() {
  Serial.begin(115200);
  const uint32_t waitStarted = millis();
  while (!Serial && millis() - waitStarted < 3000) delay(10);

  Serial.println("\nXIAO ESP32-S3 + LSM6DSOX test");
  Wire.begin(SDA_PIN, SCL_PIN, 100000);
  Wire.setTimeOut(50);
  imuReady = initializeImu();
}

void loop() {
  if (!imuReady) {
    delay(2000);
    imuReady = initializeImu();
    return;
  }

  // Check the device independently: getEvent() does not report I2C failures.
  if (!hasExpectedIdentity(imuAddress)) {
    Serial.println("Lost contact with the IMU. Check power and I2C wiring.");
    imuReady = false;
    return;
  }

  sensors_event_t accel, gyro, temperature;
  imu.getEvent(&accel, &gyro, &temperature);

  // Labeled numeric columns also work in Arduino Serial Plotter.
  Serial.print("Ax_mps2:"); Serial.print(accel.acceleration.x, 3);
  Serial.print("\tAy_mps2:"); Serial.print(accel.acceleration.y, 3);
  Serial.print("\tAz_mps2:"); Serial.print(accel.acceleration.z, 3);
  Serial.print("\tGx_dps:"); Serial.print(gyro.gyro.x * RAD_TO_DEG, 2);
  Serial.print("\tGy_dps:"); Serial.print(gyro.gyro.y * RAD_TO_DEG, 2);
  Serial.print("\tGz_dps:"); Serial.println(gyro.gyro.z * RAD_TO_DEG, 2);

  delay(100);
}
