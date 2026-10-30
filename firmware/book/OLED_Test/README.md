# Book SPI OLED test

Open `OLED_Test.ino` in Arduino IDE. This is a standalone display test in its
own folder; it needs no Hall sensors and does not initialize the other book
peripherals. Wiring and the initial controller choice follow the supplied
`main.cpp`.

## Wiring

| OLED signal | Book ESP32-S3-DevKitC-1 GPIO |
| --- | --- |
| SCK / CLK / D0 | 13 |
| SDA / DIN / D1 (SPI MOSI) | 12 |
| RES / RST | 11 |
| DC | 10 |
| CS | 9 |
| GND | GND |

These numbers are GPIO labels, not header positions. The display's D0/D1
labels are its SPI signal names, not XIAO pad names. On this connection,
**SDA is SPI data, not I2C**. There is no MISO wire or I2C address to scan.

Keep the display's existing rated power connection. If the module specifies
3.3V operation, use 3V3 for VCC; confirm its specification before changing
the supply. ESP32 signal levels are 3.3V. Make wiring changes with power off.
The marking "0.96 inch v2.3" does not establish the controller or supply
requirements on its own.

## Upload and observe

1. Select **ESP32S3 Dev Module** in Arduino IDE.
2. Use the book's **CP210x USB-to-UART connector** and its current COM port.
   This sketch logs through `Serial0`; USB CDC On Boot does not change that.
3. The Adafruit display libraries are installed on this computer. On another
   computer, install **Adafruit SSD1306** and its dependencies (**Adafruit GFX
   Library** and **Adafruit BusIO**) in Library Manager. Install **Adafruit
   SH110X** as well if using the optional SH1106 controller setting.
4. Upload this sketch and open Serial Monitor at **115200 baud**.
5. Watch the repeating eight-second cycle:
   - Text with a changing frame number and a border touching all four edges.
   - A checkerboard that changes every half-second.
   - All pixels on.
   - All pixels off. This two-second blank screen is intentional.

The test uses a 1 MHz SPI clock for jumper-wire testing. It explicitly uses
SCK=13 and MOSI=12, which differ from the generic ESP32-S3 SPI defaults.

## Controller selection and troubleshooting

`USE_SH1106` is **0** by default, selecting **SSD1306, 128x64**, just as in
your friend's sample. If your module is confirmed to use SH1106, change the
definition to `#define USE_SH1106 1` and upload again. It is not automatically
detected.

If Serial Monitor prints pattern names but the screen stays blank, check
power, common ground, all five signal wires, the controller setting, and
that the module is configured for SPI. Successful library initialization
does not prove that a display is connected: this SPI connection has no
display readback. Shifted/cropped patterns can indicate a wrong controller
or resolution setting. A completely blank Serial Monitor can mean the wrong
USB connector/COM port; use the book's CP210x connection and press reset.

After testing, upload the separate [Hall sensor test](../Hall_Sensor_Test/README.md)
to check the two magnet inputs independently. Uploading a test replaces the
sketch currently running on the board.

## Validation

Both controller settings compiled successfully with Arduino ESP32 core
3.3.12 for `esp32:esp32:esp32s3`:

| Setting | Flash | Global RAM |
| --- | --- | --- |
| SSD1306 (default) | 338,746 bytes | 23,688 bytes |
| SH1106 (optional) | 339,446 bytes | 23,704 bytes |

Installed libraries used: Adafruit SSD1306 2.5.17, Adafruit SH110X 2.1.15,
Adafruit GFX Library 1.13.0 and Adafruit BusIO 1.17.4. The actual screen and
controller have not been tested through the connected hardware.

## References

- [Adafruit SSD1306 library](https://github.com/adafruit/Adafruit_SSD1306)
- [Adafruit SH110X library](https://github.com/adafruit/Adafruit_SH110x)
- [Adafruit OLED library guide](https://learn.adafruit.com/monochrome-oled-breakouts/arduino-library-and-examples)
