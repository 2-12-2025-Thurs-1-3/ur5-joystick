#ifndef JOYSTICK_H
#define JOYSTICK_H
#include "pinout.h"

struct JoystickReading {
    double x;
    double y;
    double z;
};

struct pos_t {
    float x;
    float y;
    float z;
};

void joystick_to_xyz (pos_t* pos, JoystickReading* joystick){
    float vel = 0.01;
    if (abs(joystick -> x) > 0.1){
        pos -> x += (joystick -> x) * vel;
    };
    if (abs(joystick -> y) > 0.1){
        pos -> y += (joystick -> y) * vel;
    };
    pos -> z += (joystick -> z) * vel;
};



void setupJoystick() {
    pinMode(XPIN, INPUT); 
    pinMode(YPIN, INPUT); 
    pinMode(UP_BUTTON_PIN, INPUT_PULLUP);  
    pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP); 
};

JoystickReading readJoystick() {
    JoystickReading output;
    output.x = analogRead(XPIN)/2048.0 - 1.0; 
    output.y = analogRead(YPIN)/2048.0 - 1.0; 
    output.z = digitalRead(UP_BUTTON_PIN) - digitalRead(DOWN_BUTTON_PIN);
    return output;
};
#endif


