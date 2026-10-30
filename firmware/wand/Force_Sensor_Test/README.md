# Force sensor test

Standalone Arduino sketch for the **Seeed XIAO ESP32-S3**, with the Adafruit
#166 FSR voltage-divider junction connected to **D0 / GPIO1**.

## Wiring

```text
XIAO 3V3 ---- FSR ----+---- 10 kohm resistor ---- GND
                     |
                  D0 / GPIO1
```

Use the XIAO's 3V3 pin for the divider. Either FSR lead can face 3V3. The fixed
resistor goes between the signal junction and GND. This is an analog sensor;
it does not use SDA/SCL or I2C.

## Run it

1. Open `Force_Sensor_Test.ino` in Arduino IDE. No extra library is required.
2. Select **XIAO_ESP32S3** under **esp32 by Espressif Systems**.
3. Set **USB CDC On Boot** to **Enabled**.
4. Select the port that appears when you connect the XIAO's USB-C cable.
   This may differ from the DevKit's COM6. Upload the sketch.
5. Open **Tools > Serial Monitor** and select **115200 baud**.
6. Press and release the FSR's sensing circle. Watch `Raw` and `mV` change.
   You can use **Tools > Serial Plotter** instead for a graph.

The sketch reports roughly ten times per second and averages 16 samples per
reported value. No commands need to be sent. The USB connection is only waited
on for up to three seconds at startup; opening the monitor later still works.

## Interpret the readings

- `Raw`: ADC reading from 0 to 4095.
- `mV`: calibrated ADC voltage reading in millivolts; 1000 mV = 1 V.
- Released: normally near zero with this divider arrangement.
- Pressed: readings increase as the FSR resistance decreases.
- A raw reading near 4095 means the ADC is near its upper measurement limit.
  The ESP32-S3's documented range at this attenuation ends around 3100 mV,
  so a strong press can saturate the reading before the divider reaches 3.3 V.

These readings are not measurements of force in newtons. Record the values
while holding the wand normally and while deliberately squeezing it; those
observations can be used to choose a later "Charge It" threshold.

If readings stay near zero, check the FSR connection to 3V3 and the junction's
connection to D0. If they stay at maximum even with the FSR released, check the
resistor's connection to GND and that D0 is connected to the junction.

## References

- [Adafruit FSR divider wiring](https://learn.adafruit.com/force-sensitive-resistor-fsr/using-an-fsr)
- [Seeed XIAO ESP32-S3 pinout and setup](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/)
- [Espressif ADC API and voltage ranges](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/adc.html)
