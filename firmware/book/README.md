# Book firmware

The book uses the larger **ESP32-S3-DevKitC-1**.

| Test | Open in Arduino IDE | Setup |
| --- | --- | --- |
| DFPlayer Mini voice prompts | [MP3_Player.ino](MP3_Player/MP3_Player.ino) | [Wiring, microSD files, and controls](MP3_Player/README.md) |
| SPI OLED patterns | [OLED_Test.ino](OLED_Test/OLED_Test.ino) | [Wiring, libraries, and expected patterns](OLED_Test/README.md) |
| Two analog Hall sensors | [Hall_Sensor_Test.ino](Hall_Sensor_Test/Hall_Sensor_Test.ino) | [Wiring and magnet test](Hall_Sensor_Test/README.md) |

Each test stays in its own folder. Open and upload one `.ino` at a time;
uploading it replaces the program currently running on the book.

For the audio test, **GPIO17 is ESP32 TX**, connected through the existing
series resistor to DFPlayer RX. **GPIO18 is ESP32 RX**, connected to DFPlayer
TX. These are the native UART1 pins, selected explicitly in the sketch.
The player uses `Serial1` at 9600 baud and shares GND with the board.
Leave the header TX/RX pins (GPIO43/44, UART0) available for the USB-to-UART
connection and Serial Monitor.

**GPIO4 is reserved for the book's external NeoPixel data signal.** The LED
model and power circuit are still to be confirmed; the current MP3 sketch
does not yet control the LED.

The supplied `main.cpp` identifies **two analog Hall sensors on GPIO1 and
GPIO2**, correcting the earlier IMU description. The **0.96-inch OLED marked
v2.3 uses SPI**: SCK=13, MOSI=12, RES=11, DC=10 and CS=9. Its standalone test
defaults to SSD1306 at 128x64, matching the sample, and retains an optional
SH1106 setting. The marking alone does not confirm the controller. See the
[project pinout](../../hardware/prototype-pinout.md) for the connections.

Select **ESP32S3 Dev Module** in Arduino IDE and use the board's **CP210x
USB-to-UART connector**. The console explicitly uses `Serial0` at **115200
baud**, so it does not depend on USB CDC On Boot. Select the current CP210x
COM port; its number can change.

See the [project pinout](../../hardware/prototype-pinout.md) for power and
speaker wiring. These are standalone component tests; the complete game is
not implemented yet.
