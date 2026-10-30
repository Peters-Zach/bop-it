# Bop It voice prompts: ESP32-S3 + DFPlayer Mini

Open `MP3_Player.ino` in Arduino IDE. Keep the complete `MP3_Player` folder
together: the official DFRobot library is included under `src`, so no separate
library installation is needed.

## Prepare the microSD card

Use a FAT16/FAT32 card, 32 GB or smaller. Copy the `mp3` folder from `sd_card`
to the card's root, keeping these names:

| Path on the card | Spoken prompt |
| --- | --- |
| `/mp3/0001.mp3` | Charge It |
| `/mp3/0002.mp3` | Learn It |
| `/mp3/0003.mp3` | Cast It |

The three supplied recordings use the Microsoft Zira Desktop English voice.
They are real MP3 files, encoded at 44.1 kHz, mono, 128 kb/s. You can replace
them with your own recordings while preserving the names. Do not just rename
a WAV file to `.mp3`.

Insert the card with the DFPlayer powered off. The ESP32 sends playback
commands; the audio files stay on the DFPlayer's card, not in the ESP32 sketch.

## Native UART1 wiring

| ESP32-S3 | DFPlayer Mini |
| --- | --- |
| GPIO17, UART1 TX (U1TXD) | RX, through your existing series resistor |
| GPIO18, UART1 RX (U1RXD) | TX |
| GND | GND |

With power off, move the wire that runs through the resistor to DFPlayer RX
from GPIO47 to GPIO17. Move the wire from DFPlayer TX from GPIO21 to GPIO18.
These are GPIO labels, not positions counted along the header.

The DFPlayer needs its existing power connection (5 V recommended) and the
speaker across SPK1 and SPK2. Neither speaker lead goes to GND.

GPIO17/18 are the ESP32-S3's native UART1 pins. The board's header pins labeled
TX/RX are GPIO43/44 (UART0), shared with its CP210x USB-to-UART bridge; leave
those available for uploading and Serial Monitor. The sketch explicitly
selects GPIO18 RX and GPIO17 TX because Arduino core 3.3.12's automatic
`Serial1` defaults are GPIO15 RX and GPIO16 TX.

## Upload and test

1. Open `MP3_Player.ino` and keep your working ESP32-S3 board selection.
2. Use the same USB-to-UART connector and COM6 used for the successful Blink
   upload. If Windows changes the port number, choose its new CP210x port.
3. Upload. Once the module initializes, it plays **Charge It** once at the
   `START_VOLUME` configured in the sketch (0-30). It does not continuously
   repeat or cycle the prompts.
4. Open Serial Monitor at **115200 baud**. Send a command from the table below.
   Any line-ending setting works. If you missed startup messages, send `?`.

| Command | Action |
| --- | --- |
| `1` | Play Charge It |
| `2` | Play Learn It |
| `3` | Play Cast It |
| `s` | Stop playback |
| `+` / `-` | Raise/lower volume, bounded to 0-30 |
| `r` | Reset/reinitialize the player, then play Charge It if ready |
| `?` | Print the controls |

A new prompt replaces any prompt currently playing. Serial Monitor reports
playback completion and module errors. "Requested" means a command was sent;
it is not proof that the speaker produced sound.

The console explicitly uses `Serial0` through the CP210x USB-to-UART bridge.
The DFPlayer uses the separate `Serial1` hardware UART at 9600 baud, with RX=18
and TX=17. No onboard LED pin is used.

If initialization fails, check the card and module power, wait a few seconds,
then send `r`. A missing-file error usually means the `mp3` folder or four-digit
filenames do not match the table.

## Use in the game

Call `playChargeIt()`, `playLearnIt()`, or `playCastIt()` when the game chooses
that action. Keep initialization in `setup()` and poll `reportPlayerEvent()`
from `loop()`. The current sketch is a prompt playback test; it does not yet
contain the full game's sensor or scoring logic.

## Validation

The GPIO17/18 revision compiled successfully with Arduino ESP32 core 3.3.12
for `esp32:esp32:esp32s3`: 276,873 bytes of flash and 21,640 bytes of global
RAM. The supplied MP3 files were previously decoded successfully, and you
confirmed playback with the earlier GPIO47/21 wiring. The new GPIO17/18
revision has not yet been uploaded or tested on the physical board.

## Sources and bundled library

- [DFRobot DFPlayer hardware and microSD requirements](https://wiki.dfrobot.com/dfr0299)
- [DFRobot filename-based playback example](https://github.com/DFRobot/DFRobotDFPlayerMini/blob/master/examples/FullFunction/FullFunction.ino)
- [ESP32 hardware UART API](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/serial.html)
- [ESP32-S3-DevKitC-1 header pin functions](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/user_guide_v1.1.html#header-block)

The unmodified DFRobotDFPlayerMini 1.0.6 sources are bundled under
`src/DFRobotDFPlayerMini`, pinned to commit
`47c014938318f99f5b48194b329ef48931d1d175`. Their upstream copyright and GNU
Lesser General Public License notices are retained in the source headers and
upstream README. See `UPSTREAM.txt` for the exact source URL.
