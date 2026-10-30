# Wand haptic test

Open `Haptic_Test.ino` in Arduino IDE. This separate wand test drives the
**Adafruit DRV2605L (#2305)** and **ERM coin motor (#1201)** for half a second
every ten seconds. It runs without Serial Monitor open or any commands.

## Wiring

Make connections with power off.

| DRV2605L connection | Connect to |
| --- | --- |
| VIN | XIAO 3V3 |
| GND | XIAO GND |
| SDA | XIAO D4 / GPIO5 |
| SCL | XIAO D5 / GPIO6 |
| IN / INT / IN-TRIG (unused trigger input) | GND |
| Motor + | Motor red wire |
| Motor - | Motor blue wire |

Neither motor lead connects directly to a GPIO or GND. The driver provides
the motor power. No external motor series resistor is required.

SDA/SCL can share the wand IMU's existing wires: the DRV2605L uses address
**0x5A**, separate from the LSM6DSOX's 0x6A/0x6B. No extra XIAO GPIO is needed.
Use **3.3V VIN**, since this breakout's I2C pull-ups connect to VIN. The
driver's unused trigger input is tied to GND as specified by TI; this corrects
the earlier suggestion to leave it floating.

## Upload and test

1. Select **XIAO_ESP32S3** in Arduino IDE. Generic **ESP32S3 Dev Module** also
   works with the explicitly assigned I2C pins.
2. Set **USB CDC On Boot > Enabled**. The sketch gives a compile error if
   this is disabled, to prevent the previous blank USB monitor problem.
3. Use the XIAO's native **USB-C connection** and its current COM port.
4. The **Adafruit DRV2605 Library** and **Adafruit BusIO** are installed on
   this computer. On another computer, install Adafruit DRV2605 Library in
   Library Manager and accept its dependencies.
5. Upload. After the driver initializes, wait **10 seconds** for the first
   buzz. Subsequent buzzes start every ten seconds and last approximately
   500 ms, followed by about 9.5 seconds off.
6. Optionally open Serial Monitor at **115200 baud** for initialization,
   `Vibration ON`, and `Vibration OFF` messages. A message confirms a command
   was issued, not that physical motor movement was measured.

Serial startup waits at most three seconds for a monitor, then the test
continues on its own. If the driver is missing or its settings cannot be
verified, initialization retries every two seconds. The test uses `millis()`
timers rather than a ten-second blocking delay.

The FSR and IMU may stay connected. This sketch only controls the haptic
driver. Uploading it replaces the other test currently running on the wand.

## Adjusting the test

The constants near the top of the sketch control the timing:

- `VIBRATION_INTERVAL_MS = 10000`: time between buzz starts.
- `VIBRATION_DURATION_MS = 500`: how long each buzz is driven.
- `VIBRATION_STRENGTH = 127`: signed real-time playback level; use 1-127.
  Smaller values reduce drive, but may be too weak to start the motor.

The test uses ERM open-loop real-time playback, not LRA mode. For the listed
2.5-3.8V motor, `MOTOR_CLAMP_CODE = 138` sets approximately **2.98V** full-scale
average drive using the DRV2605L datasheet equation `0.02159 V * code`.
The drive setting is not a measured motor voltage. This open-loop test does
not need motor auto-calibration. Recheck motor settings before substituting
a different actuator, especially an LRA.

If nothing vibrates, check the 3.3V supply, common ground, SDA/SCL wiring,
and both motor connections. An I2C acknowledgement only confirms the driver
responded. Power off before changing wires. For a blank Serial Monitor,
check USB CDC, select the XIAO's current port, then tap RESET.

## Validation

Compiled successfully for `esp32:esp32:XIAO_ESP32S3` with Arduino ESP32 core
3.3.12, Adafruit DRV2605 Library 1.2.4 and Adafruit BusIO 1.17.4. The build
uses 299,857 bytes of flash and 23,296 bytes of global RAM. The sketch has not
been uploaded or tested on the physical driver and motor.

## References

- [Adafruit DRV2605L wiring](https://learn.adafruit.com/adafruit-drv2605-haptic-controller-breakout/pinouts)
- [Adafruit Arduino library](https://github.com/adafruit/Adafruit_DRV2605_Library)
- [TI DRV2605L datasheet, sections 8.5.2.1 and 8.5.8.2.1](https://www.ti.com/lit/ds/symlink/drv2605l.pdf)
- [Adafruit coin motor #1201](https://www.adafruit.com/product/1201)
