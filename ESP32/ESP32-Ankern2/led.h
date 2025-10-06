#ifndef LED_H
#define LED_H

// pin numbers for LEDs
const int redLedPin = 32;
const int yellowLedPin = 2;
const int blueLedPin = 5;
const int whiteLedPin = 4;

void setupLEDs();

//=================LED CONTROL FUNCTIONS============================================
void turnOnRedLED();
void turnOffRedLED(); 
void turnOnBlueLED();
void turnOffBlueLED();
void turnOnWhiteLED();
void turnOffWhiteLED(); 
void turnOnYellowLED();
void turnOffYellowLED();
//=========================END=====================================================
#endif