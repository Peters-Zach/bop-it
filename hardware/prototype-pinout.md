# Basic prototyping pinout

I'm using one XIAO ESP32S3 for the book and one for the wand. The larger DevKitC boards will stay as spares. The wand's FSR will connect through a fixed-resistor voltage divider.

The basic build is:

- **Book:** XIAO, page sensor, OLED, start button, MAX98357A amplifier, speaker and the XIAO's built-in status LED.
- **Wand:** XIAO, LSM6DSOX motion sensor and one FSR with a fixed resistor.

## Book: XIAO ESP32S3

**D labels identify board pads; GPIO numbers are used in the code.** All MCU signals use 3.3V logic. This pinout is for the standard XIAO ESP32S3. [XIAO pin reference](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/).

| Function | XIAO pad | GPIO | Connect to |
| --- | --- | --- | --- |
| Page Hall sensor 1 | D0 | GPIO1 | DFR1132 #1 A output |
| (Optional) page Hall sensor 2 | D1 | GPIO2 | DFR1132 #2 A output |
| (Optional) page Hall sensor 3 | D3 | GPIO4 | DFR1132 #3 A output |
| OLED I2C data | D4 | GPIO5 | OLED Data / SDA |
| OLED I2C clock | D5 | GPIO6 | OLED Clk / SCL |
| Start button/Page hinge sensor | D7 | GPIO44 | Tactile button or sensor between D7 and GND |
| Audio bit clock | D8 | GPIO7 | MAX98357A BCLK |
| Audio word clock | D9 | GPIO8 | MAX98357A LRC |
| Audio data out | D10 | GPIO9 | MAX98357A DIN |
| Valid / invalid status | Built-in LED; no external pad | GPIO21 | Already connected on the XIAO; LOW = on, HIGH = off |
| Spare | D6 | GPIO43 | Leave unconnected |
| Leave unused: strapping pin | D2 | GPIO3 | Leave unconnected |
| Sensor power | 3V3 | -- | OLED Vin and each Hall sensor VCC |
| Audio power | 5V | -- | MAX98357A VIN, with XIAO powered through USB |
| Ground | GND | -- | All book circuit grounds |

With all three Hall sensors, I'll use **9 external signal pins**. The status LED uses its own onboard GPIO21, so D6 stays spare and D2/GPIO3 stays unused. With only one Hall sensor, D1 and D3 are also free. I'll use native USB for programming and logging, and keep UART off D7 since that pin is assigned to the start button. I2S BCLK/LRC/DOUT will be GPIO7/8/9; the SPI labels on D8-D10 don't restrict their use. [Pin multiplexing](https://wiki.seeedstudio.com/xiao_esp32s3_pin_multiplexing/), [I2S configuration](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/peripherals/i2s.html).

### Valid / invalid LED

I'll use the book XIAO's built-in LED for quick feedback while testing:

- **Solid on:** valid action.
- **Blinking:** invalid action.
- **Off:** waiting for an action.

GPIO21 is active LOW, so LOW turns the LED on and HIGH turns it off. I'll return it to off when the game starts waiting for the next action. Blinking will use a timer so it doesn't pause sensor readings. This doesn't need an extra LED, resistor or external GPIO connection.

### Page sensor, button and display

- **DFR1132 is analog:** VCC to 3.3V, GND to GND, A to its assigned D0/D1/D3 input. All three are ADC1 pins. I'll use maximum ADC attenuation and set the page threshold from test readings; values may clip above approximately 3.1V. [Hall sensor](https://wiki.dfrobot.com/dfr1132/), [ESP32-S3 ADC](https://docs.espressif.com/projects/esp-idf/en/release-v4.4/esp32s3/api-reference/peripherals/adc.html).
- A listed **magnetic contact switch #375 or roller microswitch #819 can replace a Hall sensor** on the same input. Configure that input as INPUT_PULLUP instead of analog reading. Reed: between GPIO and GND. Roller: COM to GND, NO to GPIO, NC unused. Do not connect a switch and analog Hall output to the same input simultaneously.
- **Start button D7/GPIO44:** configure INPUT_PULLUP; pressed reads LOW. Debounce it and any page switches in firmware.
- I'll try the magnets included with the magnetic contact switches and check the spacing needed for the page sensors. [Contact switch](https://www.adafruit.com/product/375).
- **OLED #326:** 3.3V to Vin, GND to GND, D4 to Data/SDA, D5 to Clk/SCL. Initialize SDA=GPIO5 and SCL=GPIO6, starting at 100kHz. The STEMMA QT 128x64 model normally uses address **0x3D** and has auto-reset. Scan to confirm if jumpers have changed. [OLED guide](https://learn.adafruit.com/monochrome-oled-breakouts?view=all).

### Audio

I'll connect the **MAX98357A #3006** using the three audio signals in the table, 5V to VIN and GND to GND. GAIN and SD can stay at their breakout defaults. The FIT0502 speaker connects across amplifier + and -; **neither speaker lead goes to GND**. No MCLK connection is needed. I'll start with generated audio or audio stored in ESP32 flash and test at low volume while using USB power. [Amplifier pinout](https://learn.adafruit.com/adafruit-max98357-i2s-class-d-mono-amp/pinouts), [speaker](https://www.dfrobot.com/product-1506.html).

## Wand: XIAO ESP32S3

The wand only needs **three external signal pins**: two for motion and one for squeeze sensing. I'll use the XIAOs' built-in radios for communication between the book and wand, with a separate I2C bus on each board.

| Function | XIAO pad | GPIO | Connect to |
| --- | --- | --- | --- |
| Squeeze / Charge | D0 | GPIO1 | Junction of FSR #166 and fixed resistor |
| Motion-sensor I2C data | D4 | GPIO5 | LSM6DSOX SDA |
| Motion-sensor I2C clock | D5 | GPIO6 | LSM6DSOX SCL |
| Sensor power | 3V3 | -- | LSM6DSOX VIN and top of FSR divider |
| Ground | GND | -- | LSM6DSOX GND and bottom of divider |

    Wand 3V3 ---- FSR ----+---- fixed resistor ---- GND
                         |
                      D0 / GPIO1

I'll choose the divider resistance and squeeze threshold during testing. More force reduces FSR resistance and raises the voltage at D0. I'll configure D0 as an analog input with maximum attenuation and no internal pull-up, then compare released and squeezed readings. Strong force may saturate the ADC near its upper range. [FSR divider reference](https://learn.adafruit.com/force-sensitive-resistor-fsr/using-an-fsr).

I'll use the **LSM6DSOX #4438** at **0x6A** for Cast gestures, with SDA=GPIO5 and SCL=GPIO6. I'll poll it in the code, so no interrupt pin is needed. The other wand signal pads can stay unused for now. [IMU pinout](https://learn.adafruit.com/lsm6dsox-and-ism330dhc-6-dof-imu/pinouts).

If I add wand vibration, the **DRV2605L #2305** can share SDA/SCL at **0x5A**, powered from 3.3V. Motor #1201 red goes to driver +, blue to driver -. I'll use ERM mode with settings appropriate to its 2.5-3.8V rating. This uses no additional MCU signal pins. The motor connects to the driver, never directly to a GPIO. [Driver](https://learn.adafruit.com/adafruit-drv2605-haptic-controller-breakout/pinouts), [motor](https://www.adafruit.com/product/1201).

## Power and optional BOM parts

- Power each XIAO through USB while testing. Grounds stay common within each unit, and the two boards' 3.3V and 5V rails stay separate. Never apply 5V to a GPIO or feed external power into a board while USB is powering it. [XIAO power reference](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/).
- keep the **encoder, extra FSRs/flex sensor, slide switches and DevKitC boards** for later experiments.
- **VL53L1X #3967 (0x29)** and **MCP23017 #5346 (0x20)** can join the book's existing D4/D5 bus at 3.3V if distance sensing or more digital inputs are wanted. Polling needs no extra MCU signal pins. On the MCP23017 use A0-A6/B0-B6 for inputs; leave A7/B7 unused because of the documented input/SDA issue. [Distance sensor](https://learn.adafruit.com/adafruit-vl53l1x/pinouts), [expander](https://learn.adafruit.com/adafruit-mcp23017-i2c-gpio-expander/pinouts).
- **TMAG5273 Hall boards** are an I2C alternative to the analog DFR1132 sensors. One can share D4/D5 directly at **0x35**. For all four at their default address, add **PCA9548 #5626 (0x70)** on D4/D5 and connect one Hall board per channel 0-3. Power at 3.3V, set mux Vlogic to V+, and select exactly one channel at a time. [SparkFun guide](https://docs.sparkfun.com/SparkFun_Qwiic_Hall_Effect_Sensor_TMAG5273/print_view/), [mux](https://learn.adafruit.com/adafruit-pca9548-8-channel-stemma-qt-qwiic-i2c-multiplexer/pinouts).
- **DFPlayer DFR0299** is an alternative audio approach, not part of this pin assignment.
