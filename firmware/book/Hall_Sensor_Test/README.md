# Book Hall sensor test

Open `Hall_Sensor_Test.ino` in Arduino IDE. This standalone test reads two
analog Hall sensors on **GPIO1 and GPIO2**, matching the supplied `main.cpp`.
It does not require the OLED or any additional Arduino library.

## Wiring

The source code establishes the two analog signal pins, but does not identify
the exact sensor model. For the **DFRobot DFR1132** modules on the parts list:

| Sensor connection | ESP32-S3-DevKitC-1 connection |
| --- | --- |
| Sensor 1 A / analog output | GPIO1 |
| Sensor 2 A / analog output | GPIO2 |
| Both VCC pins | 3V3 |
| Both GND pins | GND |

Use the GPIO numbers printed on the board, not positions counted along the
header. Connect with power off. These sensors provide their own output voltage;
they do not need the FSR's external resistor divider. Do not apply 5V to an
ESP32 GPIO. If using a different Hall sensor, check its power and output
requirements before following this wiring.

## Upload and test

1. Select **ESP32S3 Dev Module** under the Espressif ESP32 boards in Arduino IDE.
2. Use the book's **CP210x USB-to-UART connector** and select its current COM
   port. The sketch explicitly uses `Serial0`, so USB CDC On Boot does not
   change its monitor connection.
3. Upload `Hall_Sensor_Test.ino`. Each test upload replaces the previous sketch
   on the board; only one standalone test runs at a time.
4. Open Serial Monitor at **115200 baud**. Readings appear about ten times per
   second as `H1_raw`, `H2_raw`, `H1_mV`, and `H2_mV`.
5. Move a magnet close to sensor 1, then away. Repeat for sensor 2. Try both
   magnet poles and compare the readings with the magnet removed. The matching
   sensor should show a repeatable change. For the DFR1132, a close magnet
   (typically within 1 cm) is a useful starting point.

## Understanding the readings

- Raw readings use 12 bits: **0-4095**. Each displayed result averages 16
  samples to reduce jitter. The mV readings are measured separately with
  `analogReadMilliVolts()`, not calculated as raw times 3300/4095.
- A linear Hall sensor need not read zero without a magnet. The two sensors
  can have different baselines, and reversing the magnet can reverse the
  direction of the change. Record each sensor's resting and activated values
  before choosing a page-detection threshold.
- The ESP32-S3 ADC's documented range at maximum attenuation is approximately
  **0-3100 mV**. A stronger signal can saturate near 4095 even with a correctly
  powered 3.3V sensor. These readings are not calibrated magnetic field units.
- A disconnected analog input can float and show changing numbers. Numbers
  alone do not prove the sensor is connected. If a sensor does not respond,
  check its signal wire, common ground, power, magnet distance and orientation.
- Serial Plotter can graph the labeled readings too. Raw counts and mV have
  different units; compare the two raw traces or the two mV traces together.

## Validation

Compiled successfully with Arduino ESP32 core 3.3.12 for
`esp32:esp32:esp32s3`: 290,745 bytes of flash and 22,240 bytes of global RAM.
Physical sensor response must be checked on the connected board.

## References

- [DFRobot DFR1132 pinout and operating range](https://wiki.dfrobot.com/dfr1132/)
- [Espressif Arduino ADC API and ESP32-S3 input range](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/adc.html)

