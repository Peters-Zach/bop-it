# Wand firmware

The wand uses the **Seeed XIAO ESP32-S3**. Each folder is a separate Arduino
sketch; upload one test at a time.

| Test | Open in Arduino IDE | Setup |
| --- | --- | --- |
| FSR squeeze input | [Force_Sensor_Test.ino](Force_Sensor_Test/Force_Sensor_Test.ino) | [Divider wiring and readings](Force_Sensor_Test/README.md) |
| LSM6DSOX motion sensor | [IMU_Test.ino](IMU_Test/IMU_Test.ino) | [Wiring, libraries, and readings](IMU_Test/README.md) |
| DRV2605L vibration motor | [Haptic_Test.ino](Haptic_Test/Haptic_Test.ino) | [Wiring and ten-second vibration test](Haptic_Test/README.md) |

- **FSR:** D0 / GPIO1 reads the junction of the FSR and a 10 kohm resistor.
  The FSR connects to 3V3; the fixed resistor connects to GND.
- **IMU:** SDA goes to D4 / GPIO5, SCL to D5 / GPIO6, VIN to 3V3, and GND
  to GND. The test checks I2C addresses 0x6A and 0x6B; no interrupt pin is used.
- **Haptic driver:** DRV2605L SDA/SCL share D4/GPIO5 and D5/GPIO6 with the IMU.
  VIN goes to 3V3, GND and the unused IN/INT trigger pin to GND. Motor red/blue
  connect to the driver's motor +/-. Its I2C address is 0x5A.

Select **XIAO_ESP32S3** in Arduino IDE and set **USB CDC On Boot > Enabled**.
Connect through the XIAO's **native USB-C port**, select its current COM port,
and open Serial Monitor at **115200 baud**. These sketches use `Serial` over
USB; the wand's port is separate from the book's CP210x port.

The FSR and IMU tests print readings continuously without commands. The
haptic test drives the motor for 500 ms every ten seconds, with the first
buzz ten seconds after initialization; it runs without Serial Monitor open.
See the
[project pinout](../../hardware/prototype-pinout.md) for the combined wand
wiring; these separate tests have not yet been combined into game firmware.
