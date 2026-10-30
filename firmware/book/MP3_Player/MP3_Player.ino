/*
  ESP32-S3 DevKitC-1 + DFPlayer Mini: Bop It voice prompts.

  Native UART1 wiring (pin names on the DFPlayer):
    ESP32 GPIO17 (TX) -> series resistor -> DFPlayer RX
    ESP32 GPIO18 (RX) <------------------ DFPlayer TX
    ESP32 GND -------------------------- DFPlayer GND

  GPIO43/44 (header TX/RX) remain reserved for USB-to-UART / Serial0.

  microSD card: FAT16/FAT32, 32 GB or smaller. Copy the supplied mp3 folder
  to the card root, so the files are /mp3/0001.mp3 through /mp3/0003.mp3.
    0001.mp3 = "Charge It"
    0002.mp3 = "Learn It"
    0003.mp3 = "Cast It"

  Upload using the same ESP32-S3 board selection and COM6 as Blink.
  Serial Monitor: 115200 baud, through the board's USB-to-UART connector.
  The bundled DFRobot library is in src; no Library Manager install needed.

  On startup, play "Charge It" once. Send 1, 2, or 3 to test each prompt.
  These functions can later be called by the game:
    playChargeIt();  playLearnIt();  playCastIt();
*/

#include <Arduino.h>
#include "src/DFRobotDFPlayerMini/DFRobotDFPlayerMini.h"

// Explicit native UART1 pins; Arduino's automatic Serial1 defaults can differ.
constexpr int DFPLAYER_RX_PIN = 18;  // U1RXD: ESP32 receives from DFPlayer TX.
constexpr int DFPLAYER_TX_PIN = 17;  // U1TXD: ESP32 sends to DFPlayer RX.
constexpr uint8_t START_VOLUME = 30; // Range: 0 (mute) to 30.

DFRobotDFPlayerMini player;
bool playerReady = false;
uint8_t volumeLevel = START_VOLUME;

void printHelp() {
  Serial0.println("\n1 = Charge It | 2 = Learn It | 3 = Cast It");
  Serial0.println("s = stop | + / - = volume | r = retry connection | ? = help");
}

void playPrompt(uint16_t track, const char *name) {
  if (!playerReady) {
    Serial0.println("Player not ready. Send r to retry initialization.");
    return;
  }

  // Explicit MP3-folder numbering avoids depending on file copy order.
  // Starting a new prompt replaces any sound that is currently playing.
  player.playMp3Folder(track);
  Serial0.print("Requested: ");
  Serial0.println(name);
}

void playChargeIt() { playPrompt(1, "Charge It"); }
void playLearnIt()  { playPrompt(2, "Learn It"); }
void playCastIt()   { playPrompt(3, "Cast It"); }

bool initializePlayer() {
  playerReady = false;
  Serial0.println("Connecting to DFPlayer Mini...");

  // Discard old responses before asking the module to reset.
  while (Serial1.available()) {
    Serial1.read();
  }

  if (!player.begin(Serial1, true, true)) {
    Serial0.println("No ready response from the DFPlayer.");
    Serial0.println("Check power, common GND, and the inserted FAT16/FAT32 card.");
    Serial0.println("UART: GPIO17 TX -> resistor -> DFPlayer RX; DFPlayer TX -> GPIO18 RX.");
    Serial0.println("Wait a few seconds, then send r to retry.");
    return false;
  }

  player.outputDevice(DFPLAYER_DEVICE_SD);
  delay(300);  // Let the module select its SD card before playback.
  player.volume(volumeLevel);
  delay(100);
  playerReady = true;
  Serial0.print("DFPlayer ready. Volume: ");
  Serial0.println(volumeLevel);
  return true;
}

void reportPlayerEvent() {
  if (!player.available()) {
    return;
  }

  const uint8_t event = player.readType();
  const uint16_t value = player.read();

  switch (event) {
    case DFPlayerPlayFinished:
      // The returned value is the module's index, not necessarily our filename.
      Serial0.println("Playback finished.");
      break;
    case DFPlayerCardRemoved:
      playerReady = false;
      Serial0.println("SD card removed. Reinsert with power off, then restart.");
      break;
    case DFPlayerCardInserted:
    case DFPlayerCardOnline:
      Serial0.println("SD card detected.");
      break;
    case TimeOut:
      Serial0.println("DFPlayer response timed out. Check power and UART wiring.");
      break;
    case WrongStack:
      Serial0.println("Invalid DFPlayer response. Check UART wiring and power.");
      break;
    case DFPlayerError:
      Serial0.print("DFPlayer error ");
      Serial0.print(value);
      if (value == FileIndexOut || value == FileMismatch) {
        Serial0.println(": check /mp3/0001.mp3, 0002.mp3, and 0003.mp3 on the card.");
      } else if (value == Busy) {
        Serial0.println(": SD card unavailable or busy.");
      } else {
        Serial0.println(". Send r to reinitialize if playback has stopped.");
      }
      break;
    default:
      break;
  }
}

void handleCommand(char command) {
  if (command == '\r' || command == '\n' || command == ' ') {
    return;  // Works with any Serial Monitor line-ending setting.
  }
  if (command == '?') {
    printHelp();
    return;
  }
  if (command == 'r' || command == 'R') {
    if (initializePlayer()) {
      playChargeIt();
    }
    return;
  }
  if (!playerReady) {
    Serial0.println("Player not ready. Send r to retry initialization.");
    return;
  }

  switch (command) {
    case '1': playChargeIt(); break;
    case '2': playLearnIt(); break;
    case '3': playCastIt(); break;
    case 's':
    case 'S':
      player.stop();
      Serial0.println("Stop requested.");
      break;
    case '+':
    case '-':
      if (command == '+' && volumeLevel < 30) ++volumeLevel;
      if (command == '-' && volumeLevel > 0) --volumeLevel;
      player.volume(volumeLevel);
      Serial0.print("Volume: ");
      Serial0.println(volumeLevel);
      break;
    default:
      printHelp();
      break;
  }
}

void setup() {
  // Serial0 always uses the CP210x USB-to-UART connection (currently COM6),
  // including if the IDE's USB CDC On Boot setting changes later.
  Serial0.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, DFPLAYER_RX_PIN, DFPLAYER_TX_PIN);
  delay(1500);  // Give the DFPlayer time to power up and read its card.

  Serial0.println("\nBop It - DFPlayer voice prompt test");
  printHelp();
  if (initializePlayer()) {
    playChargeIt();
  }
}

void loop() {
  reportPlayerEvent();
  if (Serial0.available()) {
    handleCommand(static_cast<char>(Serial0.read()));
  }
  delay(1);
}
