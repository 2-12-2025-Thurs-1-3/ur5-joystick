#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#include "joystick.h"
#include "pinout.h"

/*
JOYSTICK:

This code prints the current state of the joystick into the Serial Monitor, which is then read by a
Python script to control the UR5 arm. This code also sends the signals to turn the vacuum on/off to the
vacuum ESP32 through ESP NOW.

Warnings:
    - Check that commands are being correctly written to the joystick's Serial Monitor.
    - Check that commands are being received by the Vacuum ESP32.
*/

ControllerReading cReading;

// TODO: VACUUM ESP32 MAC ADDRESS
uint8_t broadcastAddress[] = {0xF4, 0x12, 0xFA, 0x40, 0x9A, 0xA0};

// Structure to receive data (must match sender)
typedef struct struct_message {
  bool vacuum_on;
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);
  setupController();

  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    //Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
         
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    //Serial.println("Failed to add peer");
    return;
  }
};

void loop() {
  readController(&cReading);
  convertRead(&cReading);
  Serial.printf("%.2f, %.2f, %.2f, %.2f, %.2f, %.2f,  \n", cReading.x, cReading.y, cReading.zplus, cReading.zminus, cReading.vacuum, cReading.placeholder);

  myData.vacuum_on = !cReading.vacuum;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  // if (result == ESP_OK) {
  //   Serial.println("Sent with success");
  // }
  // else {
  //   Serial.println("Error sending the data");
  // }
}
