# Basic prototyping pinout

Updated 2026-10-26: use **one XIAO ESP32S3 for the book and one for the wand**, using both XIAOs in the DigiKey BOM. The larger DevKitC boards can stay spare. The user confirmed that fixed divider resistors are already available.

The basic build is:

- **Book:** XIAO, page sensor, OLED, start button, MAX98357A amplifier and speaker.
- **Wand:** XIAO, LSM6DSOX motion sensor and one FSR with an available fixed resistor.

Vibration feedback and additional page sensors are optional. An encoder, GPIO expander and I2C multiplexer are not required for this basic build. The [project proposal](../docs/Bop%20It%20Final%20Proposal.docx) supplies the Learn/Charge/Cast action context.

## Book: XIAO ESP32S3

Use the standard XIAO ESP32S3 listed in the BOM. **D labels identify board pads; GPIO numbers are used when configuring firmware.** All MCU signals use 3.3V logic. [Seeed pin reference](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/).

| Function | XIAO pad | GPIO | Connect to |
| --- | --- | --- | --- |
| Page Hall sensor 1 | D0 | GPIO1 | DFR1132 #1 A output |
| Optional page Hall sensor 2 | D1 | GPIO2 | DFR1132 #2 A output |
| Optional page Hall sensor 3 | D3 | GPIO4 | DFR1132 #3 A output |
| OLED I2C data | D4 | GPIO5 | OLED Data / SDA |
| OLED I2C clock | D5 | GPIO6 | OLED Clk / SCL |
| Start button | D7 | GPIO44 | Tactile button #367 between D7 and GND |
| Audio bit clock | D8 | GPIO7 | MAX98357A BCLK |
| Audio word clock | D9 | GPIO8 | MAX98357A LRC |
| Audio data out | D10 | GPIO9 | MAX98357A DIN |
| Spare | D6 | GPIO43 | Leave unconnected |
| Leave unused: strapping pin | D2 | GPIO3 | Leave unconnected |
| Sensor power | 3V3 | -- | OLED Vin and each Hall sensor VCC |
| Audio power | 5V | -- | MAX98357A VIN, with XIAO powered through USB |
| Ground | GND | -- | All book circuit grounds |

This uses **9 signal pins with all three Hall sensors**, leaving D6 spare while avoiding D2/GPIO3. With only one Hall sensor, D1 and D3 are also free. Use native USB for programming/logging; D7 is assigned as a button input, so do not also initialize a UART on that pin. Explicitly configure I2S BCLK/LRC/DOUT as GPIO7/8/9; the printed SPI labels on D8-D10 do not restrict their use. [Pin multiplexing](https://wiki.seeedstudio.com/xiao_esp32s3_pin_multiplexing/), [I2S configuration](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/peripherals/i2s.html).

### Page sensor, button and display

- **DFR1132 is analog:** VCC to 3.3V, GND to GND, A to its assigned D0/D1/D3 input. All three are ADC1 pins. Use maximum ADC attenuation and calibrate the page threshold; readings may clip above approximately 3.1V. [Hall sensor](https://wiki.dfrobot.com/dfr1132/), [ESP32-S3 ADC](https://docs.espressif.com/projects/esp-idf/en/release-v4.4/esp32s3/api-reference/peripherals/adc.html).
- A listed **magnetic contact switch #375 or roller microswitch #819 can replace a Hall sensor** on the same input. Configure that input as INPUT_PULLUP instead of analog reading. Reed: between GPIO and GND. Roller: COM to GND, NO to GPIO, NC unused. Do not connect a switch and analog Hall output to the same input simultaneously.
- **Start button D7/GPIO44:** configure INPUT_PULLUP; pressed reads LOW. Debounce it and any page switches in firmware.
- The magnetic contact-switch assemblies include magnets; no standalone magnets are assumed. Test the supplied magnet with the selected Hall sensor and page geometry. [Contact switch](https://www.adafruit.com/product/375).
- **OLED #326:** 3.3V to Vin, GND to GND, D4 to Data/SDA, D5 to Clk/SCL. Initialize SDA=GPIO5 and SCL=GPIO6, starting at 100kHz. The STEMMA QT 128x64 model normally uses address **0x3D** and has auto-reset. Scan to confirm if jumpers have changed. [OLED guide](https://learn.adafruit.com/monochrome-oled-breakouts?view=all).

### Audio

Wire the **MAX98357A #3006** using the three audio signals in the table, 5V to VIN and GND to GND. Leave GAIN and SD at their breakout defaults. Connect one FIT0502 speaker across amplifier + and -; **neither speaker lead goes to GND**. No MCLK connection is needed. Generate audio or store it in ESP32 flash; begin at low volume to keep USB current modest. [Amplifier pinout](https://learn.adafruit.com/adafruit-max98357-i2s-class-d-mono-amp/pinouts), [speaker](https://www.dfrobot.com/product-1506.html).

## Wand: XIAO ESP32S3

The core wand uses **three signal pins**: two for motion and one for squeeze sensing. The two XIAOs communicate through their built-in radios; their I2C buses remain separate.

| Function | XIAO pad | GPIO | Connect to |
| --- | --- | --- | --- |
| Squeeze / Charge | D0 | GPIO1 | Junction of FSR #166 and available fixed resistor |
| Motion-sensor I2C data | D4 | GPIO5 | LSM6DSOX SDA |
| Motion-sensor I2C clock | D5 | GPIO6 | LSM6DSOX SCL |
| Sensor power | 3V3 | -- | LSM6DSOX VIN and top of FSR divider |
| Ground | GND | -- | LSM6DSOX GND and bottom of divider |

    Wand 3V3 ---- FSR ----+---- fixed resistor ---- GND
                         |
                      D0 / GPIO1

Use the fixed resistor already available; record its value when selecting the squeeze threshold. More force reduces FSR resistance and raises the junction voltage. Configure D0 as an analog input with maximum attenuation, without an internal pull-up. Calibrate released and squeezed readings; strong force may saturate the ADC near its upper range. [FSR divider reference](https://learn.adafruit.com/force-sensitive-resistor-fsr/using-an-fsr).

The **LSM6DSOX #4438** at **0x6A** detects Cast gestures. Initialize SDA=GPIO5 and SCL=GPIO6; polling requires no interrupt pin. Leave other XIAO signal pads unused for the basic wand. [IMU pinout](https://learn.adafruit.com/lsm6dsox-and-ism330dhc-6-dof-imu/pinouts).

Optional wand vibration: add **DRV2605L #2305** to the same SDA/SCL bus at **0x5A**, powered from 3.3V. Motor #1201 red goes to driver +, blue to driver -. Select ERM mode and settings appropriate to its 2.5-3.8V rating. This uses no additional MCU signal pins. Never connect the motor directly to a GPIO. [Driver](https://learn.adafruit.com/adafruit-drv2605-haptic-controller-breakout/pinouts), [motor](https://www.adafruit.com/product/1201).

## Power and optional BOM parts

- Power each XIAO through USB for bench testing. USB cables, lab wire and breadboards are bench-equipment assumptions. Keep grounds common within each wired unit, and keep the two boards' 3.3V and 5V rails separate. Never apply 5V to a GPIO or inject external supply power while USB powers that same board. [XIAO power reference](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/).
- The **encoder, extra FSRs/flex sensor, slide switches and DevKitC boards** remain available for later experiments; they are not assigned in the basic build.
- **VL53L1X #3967 (0x29)** and **MCP23017 #5346 (0x20)** can join the book's existing D4/D5 bus at 3.3V if distance sensing or more digital inputs are wanted. Polling needs no extra MCU signal pins. On the MCP23017 use A0-A6/B0-B6 for inputs; leave A7/B7 unused because of the documented input/SDA issue. [Distance sensor](https://learn.adafruit.com/adafruit-vl53l1x/pinouts), [expander](https://learn.adafruit.com/adafruit-mcp23017-i2c-gpio-expander/pinouts).
- **TMAG5273 Hall boards** are an I2C alternative to the analog DFR1132 sensors. One can share D4/D5 directly at **0x35**. For all four at their default address, add **PCA9548 #5626 (0x70)** on D4/D5 and connect one Hall board per channel 0-3. Power at 3.3V, set mux Vlogic to V+, and select exactly one channel at a time. [SparkFun guide](https://docs.sparkfun.com/SparkFun_Qwiic_Hall_Effect_Sensor_TMAG5273/print_view/), [mux](https://learn.adafruit.com/adafruit-pca9548-8-channel-stemma-qt-qwiic-i2c-multiplexer/pinouts).
- **DFPlayer DFR0299** is an alternative audio approach, not part of this pin assignment. Its microSD/TF card is not listed in the BOMs. Do not connect DFPlayer and MAX98357A outputs to the same speaker. [DFPlayer guide](https://wiki.dfrobot.com/dfr0299).
- Leave both **MPM3610 #4739** converters out of the USB setup. Their 5V output needs 6-21V input; an input supply is not specified. [Converter](https://www.adafruit.com/product/4739).

## Sources and verification

Parts were selected from ZSP11_Digkey_101526_BOM.csv, zsp11_Adafruit_101526_BOM.csv and zsp11_Sparkfun_101526_BOM.csv in the OneDrive Bop It! folder, plus the divider resistors the user confirmed are already available. Use the listed XIAO headers, QT-to-male cables and WAGO connectors as appropriate; verify available interconnects before assembly.

The GPIO assignments and electrical notes were checked against the linked manufacturer references. The book has no overlapping signal assignments and leaves GPIO3 unused. This is a proposed prototype wiring reference; the circuit has not been physically tested.
