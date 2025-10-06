#ifndef LEDS_H
#define LEDS_H

// pin numbers for LEDs
const int redLedPin = 32;
const int blueLedPin = 5;
const int whiteLedPin = 4;

// Setup function to initialize LEDs
void setupLEDs() {
  pinMode(redLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(whiteLedPin, OUTPUT);
}

// Function to turn on the red LED
void turnOnRedLED()    { digitalWrite(redLedPin, HIGH);   }
// Function to turn off the red LED
void turnOffRedLED()   { digitalWrite(redLedPin, LOW);    }
// Function to turn on the blue LED
void turnOnBlueLED() { digitalWrite(blueLedPin, HIGH);    }
// Function to turn off the blue LED
void turnOffBlueLED() { digitalWrite(blueLedPin, LOW);    }
// Function to turn on the white LED
void turnOnWhiteLED() { digitalWrite(whiteLedPin, HIGH);  }
// Function to turn off the white LED
void turnOffWhiteLED() { digitalWrite(whiteLedPin, LOW);  }

#endif