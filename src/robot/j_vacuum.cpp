#include <Arduino.h>
#include <Adafruit_BNO08x.h>
#include <esp_now.h>
#include <WiFi.h>

/*
J_VACUUM:

This Code receives on/off commands for controlling the vacuum pump by reading the Serial Monitor.
The joystick ESP32 sends these commands to the Vacuum ESP32 using ESP_NOW. Use when controlling vacuum
through Python Code.

NOTE: THE JOYSTICK WORKS AS AN ANTENNA HERE, THERE IS NO HUMAN INPUT. 
  
Warnings:
    - Check that commands are being correctly written to the joystick's Serial Monitor by Python code.
    - Check that commands are being received by the Vacuum ESP32.
*/

// TODO: VACUUM ESP32 MAC ADDRESS
uint8_t broadcastAddress[] = {0xF4, 0x12, 0xFA, 0x40, 0x9A, 0xA0};

// Struct to receive data (must match sender)
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

  String cmd = Serial.readStringUntil('\n');
  //Serial.println("Received: " + cmd);

  if (cmd == "on: True ") {
    myData.vacuum_on = 1;
  } else {
    myData.vacuum_on = 0;
  }

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

   
  // if (result == ESP_OK) {
  //   Serial.println("Sent with success");
  // }
  // else {
  //   Serial.println("Error sending the data");
  // }
}
