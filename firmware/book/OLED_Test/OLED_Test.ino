/*
  Book ESP32-S3-DevKitC-1: standalone 128x64 SPI OLED test.
  Wiring follows the supplied main.cpp:
    OLED SCK / D0 -> GPIO13       OLED SDA / D1 -> GPIO12 (SPI MOSI)
    OLED RES      -> GPIO11       OLED DC       -> GPIO10
    OLED CS       -> GPIO9        OLED GND      -> common GND
  Use the display module's rated power supply and 3.3V-compatible logic.

  Open this file in Arduino IDE with the OLED_Test folder kept together.
  Select ESP32S3 Dev Module; use the CP210x USB-to-UART connector.
  Serial Monitor: 115200 baud. No Hall sensors are needed for this test.

  The screen cycles through text, checkerboard, all pixels on, and all
  pixels off. Each stage lasts two seconds; a blank stage is intentional.
*/

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>

// Matches your friend's default. Change to 1 for a confirmed SH1106 display.
// The "0.96 inch v2.3" label alone does not identify the controller.
#ifndef USE_SH1106
#define USE_SH1106 0
#endif

#if USE_SH1106 != 0 && USE_SH1106 != 1
#error "USE_SH1106 must be 0 (SSD1306) or 1 (SH1106)."
#endif

constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 64;
constexpr int8_t OLED_SCK = 13;
constexpr int8_t OLED_MOSI = 12;
constexpr int8_t OLED_RES = 11;
constexpr int8_t OLED_DC = 10;
constexpr int8_t OLED_CS = 9;
constexpr uint32_t OLED_SPI_HZ = 1000000;  // Conservative for jumper wiring.
constexpr uint32_t STAGE_MS = 2000;
constexpr uint32_t FRAME_MS = 100;
constexpr uint16_t PIXEL_ON = 1;

#if USE_SH1106
#include <Adafruit_SH110X.h>
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC,
                        OLED_RES, OLED_CS, OLED_SPI_HZ);
const char *const CONTROLLER_NAME = "SH1106";
#else
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC,
                        OLED_RES, OLED_CS, OLED_SPI_HZ);
const char *const CONTROLLER_NAME = "SSD1306";
#endif

bool displayReady = false;
uint32_t testStartedAt = 0;
uint32_t frameNumber = 0;
uint8_t previousStage = 255;

void drawTextPattern() {
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, PIXEL_ON);
  display.setTextSize(2);
  display.setCursor(10, 5);
  display.print("BOOK OLED");
  display.setTextSize(1);
  display.setCursor(6, 27);
  display.print(CONTROLLER_NAME);
  display.print(" 128x64");
  display.setCursor(6, 39);
  display.print("SPI display test");
  display.setCursor(6, 51);
  display.print("Frame: ");
  display.print(frameNumber);
}

void drawCheckerboard(bool alternate) {
  for (int16_t y = 0; y < SCREEN_HEIGHT; y += 8) {
    for (int16_t x = 0; x < SCREEN_WIDTH; x += 8) {
      if (((x / 8 + y / 8 + alternate) % 2) == 0) {
        display.fillRect(x, y, 8, 8, PIXEL_ON);
      }
    }
  }
}

void setup() {
  Serial0.begin(115200);
  delay(300);
  Serial0.println("\nBook - standalone SPI OLED test");
  Serial0.println("SCK=13, MOSI/SDA=12, RES=11, DC=10, CS=9");
  Serial0.print("Selected controller: ");
  Serial0.println(CONTROLLER_NAME);

  // No MISO wire: this OLED connection sends data to the display only.
  if (!SPI.begin(OLED_SCK, -1, OLED_MOSI, OLED_CS)) {
    Serial0.println("Could not initialize the ESP32 SPI bus.");
    return;
  }
  delay(250);

#if USE_SH1106
  // Address is ignored for SPI. The initialized ESP32 SPI bus keeps its pins.
  displayReady = display.begin(0, true);
#else
  // Keep our custom SPI pin mapping; reset the display through GPIO11.
  displayReady = display.begin(SSD1306_SWITCHCAPVCC, 0, true, false);
#endif
  if (!displayReady) {
    Serial0.println("Display library initialization failed.");
    return;
  }

  display.clearDisplay();
  display.setTextWrap(false);
  display.setTextColor(PIXEL_ON);
  display.display();
  testStartedAt = millis();
  Serial0.println("Initialization calls completed. Check the screen visually.");
  Serial0.println("SPI cannot confirm a connected display or its controller.");
  Serial0.println("Cycle: text -> checkerboard -> all on -> all off (2s each).");
}

void loop() {
  if (!displayReady) {
    Serial0.println("Test stopped: initialization failed. Check setup, then reset.");
    delay(2000);
    return;
  }

  const uint32_t elapsed = millis() - testStartedAt;
  const uint8_t stage = (elapsed / STAGE_MS) % 4;
  if (stage != previousStage) {
    const char *const stageNames[] = {
        "Text and border", "Checkerboard", "All pixels ON", "All pixels OFF"};
    Serial0.print("Pattern: ");
    Serial0.println(stageNames[stage]);
    previousStage = stage;
  }

  display.clearDisplay();
  switch (stage) {
    case 0: drawTextPattern(); break;
    case 1: drawCheckerboard((elapsed / 500) % 2); break;
    case 2: display.fillScreen(PIXEL_ON); break;
    case 3: break;  // Intentionally blank to check that pixels turn off.
  }
  display.display();
  ++frameNumber;
  delay(FRAME_MS);
}
