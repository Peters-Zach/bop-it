/*
  One LED: blink only while the push button is held.
  A new press starts with the LED ON. Releasing turns it OFF immediately.

  Bare ATmega328P, DIP-28, Arduino Uno pin numbering, 16 MHz clock:
    Button: D2 (physical pin 4) to GND; internal pull-up is enabled.
    LED:    D8 (physical pin 14) -> 330 ohm resistor -> LED anode (+).
            LED cathode (-) -> GND.
  Timing: 250 ms ON, 150 ms OFF. No blocking delays.
*/

const byte BUTTON_PIN = 2;
const byte LED_PIN = 8;
const unsigned long ON_MS = 250;
const unsigned long OFF_MS = 150;

bool wasHeld = false;
bool ledOn = false;
unsigned long phaseStarted = 0;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  // Keep the other two LEDs off if the three-LED circuit is already wired.
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
}

void loop() {
  const bool held = (digitalRead(BUTTON_PIN) == LOW);
  const unsigned long now = millis();

  if (!held) {
    digitalWrite(LED_PIN, LOW);
    ledOn = false;
    wasHeld = false;
    return;
  }

  if (!wasHeld) {
    wasHeld = true;
    ledOn = true;
    phaseStarted = now;
    digitalWrite(LED_PIN, HIGH);
  }

  const unsigned long phaseDuration = ledOn ? ON_MS : OFF_MS;
  if (now - phaseStarted >= phaseDuration) {
    ledOn = !ledOn;
    phaseStarted = now;
    digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
  }
}
