/*
  Three LEDs: repeat LED 1 -> LED 2 -> LED 3 while the button is held.
  If released during a cycle, finish that entire three-LED cycle, then stop.
  A fresh press while stopped starts again at LED 1.

  Bare ATmega328P, DIP-28, Arduino Uno pin numbering, 16 MHz clock:
    Button: D2 (physical pin 4) to GND; internal pull-up is enabled.
    LED 1:  D8 (physical pin 14).
    LED 2:  D9 (physical pin 15).
    LED 3: D10 (physical pin 16).
  Each LED pin -> its own 330 ohm resistor -> LED anode (+).
  Each LED cathode (-) -> GND.
  Timing: each LED is ON for 250 ms, followed by a 150 ms dark gap.
  One complete cycle takes 1.2 seconds.
*/

const byte BUTTON_PIN = 2;
const byte LED_PINS[] = {8, 9, 10};
const unsigned long ON_MS = 250;
const unsigned long GAP_MS = 150;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  for (byte i = 0; i < 3; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    // Check the button only between complete cycles, so a release never
    // interrupts LED 1 -> LED 2 -> LED 3.
    for (byte i = 0; i < 3; i++) {
      digitalWrite(LED_PINS[i], HIGH);
      delay(ON_MS);
      digitalWrite(LED_PINS[i], LOW);
      delay(GAP_MS);
    }
  }
}
