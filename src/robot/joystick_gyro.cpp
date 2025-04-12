#include <Arduino.h>
#include <Adafruit_BNO08x.h>

#include "joystick.h"
#include "pinout.h"

ControllerReading cReading;

void setup() {
  Serial.begin(115200);
  setupController();
};

void loop() {
  readController(&cReading);
  convertRead(&cReading);
  Serial.printf("%.2f, %.2f, %.2f, %.2f, %.2f, %.2f,  \n", cReading.x, cReading.y, cReading.zplus, cReading.zminus, cReading.yawplus, cReading.yawminus);
}
