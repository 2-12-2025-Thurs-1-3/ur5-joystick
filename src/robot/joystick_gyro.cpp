#include <Arduino.h>
#include <Adafruit_BNO08x.h>

#include "joystick.h"
#include "pinout.h"

#define BNO08X_CS 0
#define BNO08X_INT 9
#define BNO08X_RESET 5

struct euler_t {
  float yaw;
  float pitch;
  float roll;
} ypr; 

int init_b = 1;
euler_t ypr_init;

JoystickReading joystick;
pos_t pos;

// struct accel_t {
//   float x;
//   float y;
//   float z;
// } accel;
// struct pos_t {
//   float x;
//   float y;
//   float z;
// } pos;
// struct vel_t {
//   float x;
//   float y;
//   float z;
// } vel;

Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

// Quaternion-based rotation reports
sh2_SensorId_t reportType = SH2_ARVR_STABILIZED_RV;
long reportIntervalUs = 5000;

// // Accelerometer report
// sh2_SensorId_t accelReportType = SH2_LINEAR_ACCELERATION;
// long accelReportIntervalUs = 5000;

void setReports() {
  Serial.println("Setting desired reports");

  if (!bno08x.enableReport(reportType, reportIntervalUs)) {
    Serial.println("Could not enable rotation vector");
  }

  // if (!bno08x.enableReport(accelReportType, accelReportIntervalUs)) {
  //   Serial.println("Could not enable accelerometer data");
  // }
}

void setup() {
  while (!Serial) delay(10);

  Serial.println("Adafruit BNO08x test!");

  if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT)) {
    Serial.println("Failed to find BNO08x chip");
    while (1) { delay(10); }
  }
  Serial.println("BNO08x Found!");

  setReports();
  setupJoystick();

  Serial.println("Reading events");
  delay(100);
  // pos.x = 0;
  // pos.y = 0;
  // pos.z = 0;
  // vel.x = 0;
  // vel.y = 0;
  // vel.z = 0;
}

void quaternionToEuler(float qr, float qi, float qj, float qk, euler_t* ypr, bool degrees = false) {
  float sqr = sq(qr);
  float sqi = sq(qi);
  float sqj = sq(qj);
  float sqk = sq(qk);

  ypr->yaw = atan2(2.0 * (qi * qj + qk * qr), (sqi - sqj - sqk + sqr));
  ypr->pitch = asin(-2.0 * (qi * qk - qj * qr) / (sqi + sqj + sqk + sqr));
  ypr->roll = atan2(2.0 * (qj * qk + qi * qr), (-sqi - sqj + sqk + sqr));

  if (degrees) {
    ypr->yaw *= RAD_TO_DEG;
    ypr->pitch *= RAD_TO_DEG;
    ypr->roll *= RAD_TO_DEG;
  }
}

void quaternionToEulerRV(sh2_RotationVectorWAcc_t* rotational_vector, euler_t* ypr, bool degrees = false) {
  quaternionToEuler(rotational_vector->real, rotational_vector->i, rotational_vector->j, rotational_vector->k, ypr, degrees);
}

void loop() {
  if (bno08x.wasReset()) {
    Serial.println("Sensor was reset");
    setReports();
  }

  if (bno08x.getSensorEvent(&sensorValue)) {
    switch (sensorValue.sensorId) {
      case SH2_ARVR_STABILIZED_RV:
        quaternionToEulerRV(&sensorValue.un.arvrStabilizedRV, &ypr, true);

        if (init_b){
          ypr_init.yaw = ypr.yaw;
          ypr_init.pitch = ypr.pitch;
          ypr_init.roll = ypr.roll;
          init_b = 0;
        } else {
          ypr.yaw -= ypr_init.yaw;
          ypr.pitch -= ypr_init.pitch;
          ypr.roll -= ypr_init.roll;
        }
        
        joystick = readJoystick();
        joystick_to_xyz(&pos, &joystick);
        Serial.printf("X: %.2f, Y: %.2f, Z: %.2f, Yaw: %.2f, Pitch %.2f, Roll: %.2f\n", pos.x, pos.y, pos.z, ypr.yaw, ypr.pitch, ypr.roll);
        break;

      // case SH2_LINEAR_ACCELERATION:
      //   accel.x = sensorValue.un.linearAcceleration.x;
      //   accel.y = sensorValue.un.linearAcceleration.y;
      //   accel.z = sensorValue.un.linearAcceleration.z;

      //   vel.x += accel.x * reportIntervalUs * 1e-6;
      //   vel.y += accel.y * reportIntervalUs * 1e-6;
      //   vel.z += accel.z * reportIntervalUs * 1e-6;

      //   pos.x += vel.x * reportIntervalUs * 1e-6;
      //   pos.y += vel.y * reportIntervalUs * 1e-6;
      //   pos.z += vel.z * reportIntervalUs * 1e-6;

      //   // Serial.print("Acceleration (m/s²): ");
      //   Serial.print(pos.x); Serial.print("\t");
      //   Serial.print(pos.y); Serial.print("\t");
      //   Serial.println(pos.z);

      //   break;

    }
  }
}