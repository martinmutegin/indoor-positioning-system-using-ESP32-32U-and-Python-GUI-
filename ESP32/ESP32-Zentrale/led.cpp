#include "led.h"
#include "arduino.h"

// Setup function to initialize LEDs
void setupLEDs() {
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(whiteLedPin, OUTPUT);
}

void turnOnRedLED()    { digitalWrite(redLedPin, HIGH);   }
void turnOffRedLED()   { digitalWrite(redLedPin, LOW);    }
void turnOnGreenLED()  { digitalWrite(greenLedPin, HIGH); }
void turnOffGreenLED() { digitalWrite(greenLedPin, LOW);  }
void turnOnBlueLED()   { digitalWrite(blueLedPin, HIGH);  }
void turnOffBlueLED()  { digitalWrite(blueLedPin, LOW);   }
void turnOnWhiteLED()  { digitalWrite(whiteLedPin, HIGH); }
void turnOffWhiteLED() { digitalWrite(whiteLedPin, LOW);  }


