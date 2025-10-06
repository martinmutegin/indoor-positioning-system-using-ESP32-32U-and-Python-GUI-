#ifndef LED_H
#define LED_H

// pin numbers for LEDs
const int redLedPin = 32;
const int greenLedPin = 2;
const int blueLedPin = 5;
const int whiteLedPin = 4;

void setupLEDs();

//=================LED CONTROL FUNCTIONS============================================
void turnOnRedLED(); 
void turnOffRedLED();  
void turnOnGreenLED(); 
void turnOffGreenLED();
void turnOnBlueLED();
void turnOffBlueLED(); 
void turnOnWhiteLED(); 
void turnOffWhiteLED();
//========================END=======================================================

#endif