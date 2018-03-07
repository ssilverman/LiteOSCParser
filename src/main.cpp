// main.cpp is part of LiteOSCParser.
// (c) 2018 Shawn Silverman

// Other includes
#include <Arduino.h>

// --------------------------------------------------------------------------
//  Main program
// --------------------------------------------------------------------------

void runTests();

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 2000) {
  }
  delay(4000);
}

void loop() {
  runTests();
}
