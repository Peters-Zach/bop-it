# XIAO ESP32-S3 IMU test

Standalone Arduino test for the **Adafruit LSM6DSOX (#4438)** on the
**Seeed XIAO ESP32-S3**. Open `IMU_Test.ino` in Arduino IDE.

## Wiring

| IMU pin | XIAO connection |
| --- | --- |
| VIN | 3V3 |
| GND | GND |
| SDA | D4 / GPIO5 |
| SCL | D5 / GPIO6 |

The breakout already has I2C pull-up resistors. No interrupt connection is
needed; leave INT1, INT2, and the SPI/auxiliary pins unconnected. GPIO4 (D3)
is not used. The FSR can stay connected to D0; this sketch does not read it.

## Arduino IDE setup

1. Select **XIAO_ESP32S3** under **esp32 by Espressif Systems**.
   The generic **ESP32S3 Dev Module** selection can also run this test.
2. Set **USB CDC On Boot > Enabled**. The sketch intentionally gives a clear
   compile error if this is disabled, to prevent a blank USB Serial Monitor.
3. Select the XIAO's current USB port under **Tools > Port**. It was COM7
   during earlier testing, but Windows may assign another number.
4. Upload, then open **Serial Monitor at 115200 baud**.
5. Tilt and rotate the IMU. Output appears continuously without commands.

Required libraries are **Adafruit LSM6DS**, **Adafruit BusIO**, and
**Adafruit Unified Sensor**. These are already installed in your Arduino
sketchbook on this computer. On another computer, install **Adafruit LSM6DS**
from Library Manager and accept its dependencies.

## What the readings mean

- `Ax_mps2`, `Ay_mps2`, `Az_mps2`: acceleration along the sensor's three axes,
  in metres per second squared. Gravity is included. Stationary total
  acceleration should be around 9.8 m/s^2; tilting shifts that between axes.
- `Gx_dps`, `Gy_dps`, `Gz_dps`: rotation rates in degrees per second. These
  should be near zero while still and change while you rotate the sensor.
  They are rates, not rotation angles.

The accelerometer is configured for +/-4 g, the gyroscope for +/-500 deg/s,
and both run at 104 Hz. The sketch prints about ten readings per second.
Serial Plotter can graph the same output, although acceleration and gyro
columns use different units. No zero-offset calibration is applied.

The sketch checks both supported I2C addresses, 0x6A and 0x6B, and verifies
the LSM6DSOX identity. If initialization fails or contact is lost, it prints
a wiring hint and retries every two seconds. Power off before changing wires.

If the monitor is blank, check USB CDC, reselect the running sketch's USB
port, and tap RESET once. If uploading reports a busy port, close Serial
Monitor and Serial Plotter before retrying.

## Validation

Compiled successfully for `esp32:esp32:XIAO_ESP32S3` with Arduino ESP32 core
3.3.12, Adafruit LSM6DS 4.7.4, BusIO 1.17.4, and Unified Sensor 1.1.15.
The build uses 305,925 bytes of flash and 23,376 bytes of global RAM.
The sketch has not been uploaded or tested against the physical IMU.

## References

- [Adafruit wiring and Arduino library guide](https://learn.adafruit.com/lsm6dsox-and-ism330dhc-6-dof-imu/arduino)
- [Adafruit breakout pinout](https://learn.adafruit.com/lsm6dsox-and-ism330dhc-6-dof-imu/pinouts)
- [ST LSM6DSOX datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf)
- [XIAO ESP32-S3 setup and pinout](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/)
