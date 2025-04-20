#ifndef JOYSTICK_H
#define JOYSTICK_H
#include "pinout.h"

struct ControllerReading {
    double x;
    double y;
    double zplus;
    double zminus;
    double vacuum;
    double placeholder;
};

void setupController() {
    pinMode(XPIN, INPUT); 
    pinMode(YPIN, INPUT); 
    pinMode(ZPLUS_BUTTON_PIN, INPUT_PULLUP);  
    pinMode(ZMINUS_BUTTON_PIN, INPUT_PULLUP); 
    pinMode(VACUUM_BUTTON_PIN, INPUT_PULLUP);  
    pinMode(PLACEHOLDER_BUTTON_PIN, INPUT_PULLUP); 
};

void readController(ControllerReading *cr) {
    (cr -> x) = analogRead(XPIN)/2048.0 - 1.0; 
    (cr -> y) = analogRead(YPIN)/2048.0 - 1.0; 
    (cr -> zplus) = digitalRead(ZPLUS_BUTTON_PIN);
    (cr -> zminus) = digitalRead(ZMINUS_BUTTON_PIN);
    (cr -> vacuum) = digitalRead(VACUUM_BUTTON_PIN);
    (cr -> placeholder) = digitalRead(PLACEHOLDER_BUTTON_PIN);
};


void convertRead(ControllerReading *cr){
    if (abs(cr -> x) < 0.3) (cr -> x) = 0.00;
    if (abs(cr -> y) < 0.3) (cr -> y) = 0.00;
};
#endif


