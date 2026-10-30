# Basic prototyping pinout

The book uses the larger **ESP32-S3-DevKitC-1**. The wand uses the **Seeed XIAO ESP32S3**. The wand's FSR connects through a fixed-resistor voltage divider.

The basic build is:

- **Book:** ESP32-S3-DevKitC-1, DFPlayer Mini and speaker, two analog Hall sensors on GPIO1/2, and a 0.96-inch SPI OLED marked v2.3 on GPIO9-13. GPIO4 is selected for a planned external NeoPixel. The start button remains planned with its pin unassigned.
- **Wand:** XIAO, LSM6DSOX motion sensor, one FSR with a fixed resistor, and a DRV2605L driver with an ERM vibration motor.

## Book: ESP32-S3-DevKitC-1

The current audio wiring uses the **DFPlayer Mini DFR0299**. UART RX/TX names below are from the ESP32's perspective. All ESP32 GPIO signals use 3.3V logic.

| Function | DevKitC connection | DFPlayer / speaker connection |
| --- | --- | --- |
| Send playback commands | GPIO17, ESP32 UART1 TX (U1TXD) | Through the existing series resistor to DFPlayer RX |
| Receive player responses | GPIO18, ESP32 UART1 RX (U1RXD) | DFPlayer TX |
| Player power | 5V | DFPlayer VCC |
| Common ground | GND | DFPlayer GND |
| Speaker output | No ESP32 GPIO connection | Speaker across DFPlayer SPK1 and SPK2; neither speaker lead goes to GND |

Program and monitor the book through its **CP210x USB-to-UART connector**. The audio test uses `Serial0` for the monitor and `Serial1` at 9600 baud for the DFPlayer, with ESP32 RX=GPIO18 and TX=GPIO17 explicitly selecting the native UART1 pins. Leave the header TX/RX pins (GPIO43/44, UART0) available for uploading and Serial Monitor. [DevKitC header functions](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/user_guide_v1.1.html#header-block).

Open [MP3_Player.ino](../firmware/book/MP3_Player/MP3_Player.ino). Its [setup instructions and supplied audio](../firmware/book/MP3_Player/README.md) describe the microSD card and voice-prompt files. [DFPlayer reference](https://wiki.dfrobot.com/dfr0299).

### External addressable LED

**GPIO4 is selected for the book's NeoPixel data signal.** Reserve it for the LED's DIN connection; configure pin `4` in the future NeoPixel code. The current MP3 test does not yet drive this LED. The LED model is not selected, so its power, level-shifting and decoupling requirements still need to be checked. Share GND with the DevKitC and power the LED from its appropriate supply, not GPIO4.

The MAX98357A is not used in this current book audio test. The earlier XIAO book design below is retained only as a reference for components and wiring that still need to be reassigned to the DevKitC.

### Hall sensors and SPI OLED

The supplied `main.cpp` identifies the two book sensors as **analog Hall sensors**, correcting the earlier IMU description. Its wiring maps to the GPIO numbers below. The wand's XIAO/LSM6DSOX remains a separate unit.

| Device signal | DevKitC GPIO | Notes |
| --- | --- | --- |
| Hall sensor 1 analog output | 1 | Input without internal pull-up; ADC1 |
| Hall sensor 2 analog output | 2 | Input without internal pull-up; ADC1 |
| OLED SCK / CLK / D0 | 13 | SPI clock |
| OLED SDA / DIN / D1 | 12 | SPI MOSI; this SDA label is not an I2C connection |
| OLED RES / RST | 11 | Display reset |
| OLED DC | 10 | Data/command selection |
| OLED CS | 9 | SPI chip select |

Reserve GPIO1/2 for the Hall sensors and GPIO9-13 for the OLED. Share GND within the book. For the listed DFR1132 Hall modules, use 3.3V VCC and connect A to the assigned input; the exact installed sensor model remains to be confirmed. No ESP32 input may exceed its 3.3V supply. Confirm the OLED module's rated supply before changing its existing power connection.

The sample selects SSD1306 at 128x64 and provides an SH1106 alternative; the marking "0.96 inch v2.3" alone does not identify the controller. The new [OLED_Test](../firmware/book/OLED_Test/README.md) retains those choices and uses the explicit SPI pins above. The separate [Hall_Sensor_Test](../firmware/book/Hall_Sensor_Test/README.md) reads both analog inputs through Serial Monitor, without requiring the OLED. Both tests use `Serial0` at 115200 baud through the book's CP210x USB-to-UART connection. Each is a standalone sketch in its own folder; the MP3 test remains separate.

## Superseded book reference: XIAO ESP32S3 + MAX98357A

**Historical plan only: the table and subsections in this section are for the former XIAO-based book, not the current DevKitC book. Do not carry these pin assignments over to the DevKitC.** Use the current book connections and pending details above; this table does not describe the newly reported OLED wiring.

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

The FSR, IMU and haptic driver together need **three external signal pins**: two shared I2C lines for the IMU/haptic driver and one for squeeze sensing. The planned wireless link will use the XIAO and DevKitC's built-in radios.

Program and monitor the wand through the XIAO's **native USB-C connection**, using `Serial` with **USB CDC On Boot enabled**. Its USB port may differ from the book's CP210x port.

The separate tests are [Force_Sensor_Test.ino](../firmware/wand/Force_Sensor_Test/Force_Sensor_Test.ino), [IMU_Test.ino](../firmware/wand/IMU_Test/IMU_Test.ino), and [Haptic_Test.ino](../firmware/wand/Haptic_Test/Haptic_Test.ino). Each has its own folder and setup instructions; none requires the book to be connected.

| Function | XIAO pad | GPIO | Connect to |
| --- | --- | --- | --- |
| Squeeze / Charge | D0 | GPIO1 | Junction of FSR #166 and 10 kohm resistor |
| Shared I2C data | D4 | GPIO5 | LSM6DSOX SDA and DRV2605L SDA |
| Shared I2C clock | D5 | GPIO6 | LSM6DSOX SCL and DRV2605L SCL |
| Sensor/driver power | 3V3 | -- | LSM6DSOX VIN, DRV2605L VIN and top of FSR divider |
| Ground | GND | -- | LSM6DSOX GND, DRV2605L GND and unused IN/INT, and bottom of divider |

    Wand 3V3 ---- FSR ----+---- 10 kohm resistor ---- GND
                         |
                      D0 / GPIO1

The current FSR test uses a 10 kohm divider resistor. More force reduces FSR resistance and raises the voltage at D0. The test configures D0 as an analog input with maximum attenuation and no internal pull-up. Compare released and squeezed readings to choose the eventual Charge It threshold. Strong force may saturate the ADC near its upper range; the readings are not calibrated force measurements. [FSR divider reference](https://learn.adafruit.com/force-sensitive-resistor-fsr/using-an-fsr).

The **LSM6DSOX #4438** uses SDA=GPIO5 and SCL=GPIO6. Its default address is **0x6A**; the IMU test also checks **0x6B** if the address jumper is set. The code polls the sensor, so no interrupt pin is needed. The other wand signal pads can stay unused for now. [IMU pinout](https://learn.adafruit.com/lsm6dsox-and-ism330dhc-6-dof-imu/pinouts).

The **DRV2605L #2305** shares SDA/SCL at **0x5A**, powered from 3.3V. Motor #1201 red goes to driver motor +, blue to motor -; neither lead goes directly to GND or a GPIO. Tie unused IN/INT to GND as specified by TI. This uses no additional MCU signal pins. [Driver](https://learn.adafruit.com/adafruit-drv2605-haptic-controller-breakout/pinouts), [motor](https://www.adafruit.com/product/1201), [TI datasheet](https://www.ti.com/lit/ds/symlink/drv2605l.pdf).

The separate [Haptic_Test](../firmware/wand/Haptic_Test/README.md) uses ERM open-loop real-time playback at approximately 3V full-scale drive, appropriate for the listed motor's 2.5-3.8V rating. It commands a 500 ms buzz every 10 seconds, starting 10 seconds after driver initialization, without requiring Serial Monitor to be open.

## Power and optional BOM parts

- Power the book DevKitC and wand XIAO through their respective USB connections while testing. Grounds stay common within each unit, and the two boards' 3.3V and 5V rails stay separate. Never apply 5V to a GPIO or feed external power into a board while USB is powering it. [XIAO power reference](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/).
- Keep the **encoder, extra FSRs/flex sensor, slide switches, unused XIAO and additional DevKitC** for later experiments. One DevKitC is now the active book board.
- **Unconfirmed book expansion reference:** **VL53L1X #3967 (0x29)** and **MCP23017 #5346 (0x20)** could share a future book I2C bus at 3.3V. The former XIAO D4/D5 assignment is superseded; DevKitC bus pins remain unassigned. Polling needs no interrupt GPIO. On the MCP23017 use A0-A6/B0-B6 for inputs; leave A7/B7 unused because of the documented input/SDA issue. [Distance sensor](https://learn.adafruit.com/adafruit-vl53l1x/pinouts), [expander](https://learn.adafruit.com/adafruit-mcp23017-i2c-gpio-expander/pinouts).
- **Unconfirmed book expansion reference:** **TMAG5273 Hall boards** are an I2C alternative to the analog DFR1132 sensors. One uses **0x35**. For all four at their default address, a **PCA9548 #5626 (0x70)** can provide separate channels 0-3. Power at 3.3V, set mux Vlogic to V+, and select one channel at a time. DevKitC I2C pins still need to be assigned. [SparkFun guide](https://docs.sparkfun.com/SparkFun_Qwiic_Hall_Effect_Sensor_TMAG5273/print_view/), [mux](https://learn.adafruit.com/adafruit-pca9548-8-channel-stemma-qt-qwiic-i2c-multiplexer/pinouts).
