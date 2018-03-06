// tests.cpp is part of OSCParser.
// (c) 2018 Shawn Silverman

// C++ includes
#include <cstdint>

// Other includes
#include <ArduinoUnit.h>

// Project includes
#include "OSCParser.h"

OSCParser osc;

// The tests
#include "tests/address.inc"
#include "tests/args.inc"
#include "tests/match.inc"
#include "tests/packet.inc"

void runTests() {
  Test::run();
}
