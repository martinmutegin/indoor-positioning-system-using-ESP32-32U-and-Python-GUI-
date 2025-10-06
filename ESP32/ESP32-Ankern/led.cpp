#include "led.h"
#include <Arduino.h>

// Setup function to initialize LEDs
void setupLEDs() {
  pinMode(redLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(whiteLedPin, OUTPUT);
}


void turnOnRedLED()     {digitalWrite(redLedPin, HIGH);     }

void turnOffRedLED()    {digitalWrite(redLedPin, LOW);      }

void turnOnBlueLED()    {digitalWrite(blueLedPin, HIGH);    }

void turnOffBlueLED()   {digitalWrite(blueLedPin, LOW);     }

void turnOnWhiteLED()   {digitalWrite(whiteLedPin, HIGH);   }

void turnOffWhiteLED()  { digitalWrite(whiteLedPin, LOW);   }

void turnOnYellowLED()  { digitalWrite(yellowLedPin, HIGH); }

void turnOffYellowLED() {digitalWrite(yellowLedPin, LOW);   }