// tests.cpp is part of LiteOSCParser.
// (c) 2018-2019 Shawn Silverman

// C++ includes
#ifdef __has_include
#if __has_include(<cstdint>)
#include <cstdint>
#else
#include <stdint.h>
#endif
#else
#include <cstdint>
#endif

// Other includes
#include <Arduino.h>
#include <ArduinoUnit.h>

// Project includes
#include "LiteOSCParser.h"

::qindesign::osc::LiteOSCParser osc{64, 4};

#if defined(CORE_TEENSY) && !defined(TEENSYLC)
#ifdef __cplusplus
extern "C" {
#endif
// Modified from:
// https://forum.pjrc.com/threads/186-Teensy-3-fault-handler-demonstration
void __attribute__((naked)) hard_fault_isr() {
  uint32_t* sp = 0;
  // this is from "Definitive Guide to the Cortex M3" pg 423
  asm volatile(
      "TST LR, #0x4\n\t"   // Test EXC_RETURN number in LR bit 2
      "ITE EQ\n\t"         // if zero (equal) then
      "MRSEQ %0, MSP\n\t"  //   Main Stack was used, put MSP in sp
      "MRSNE %0, PSP\n\t"  // else Process stack was used, put PSP in sp
      : "=r"(sp)
      :
      : "cc");

  Serial.print("!!!! Hard Crashed at pc=0x");
  Serial.print(sp[6], 16);
  Serial.print(", lr=0x");
  Serial.print(sp[5], 16);
  Serial.println(".");
  Serial.flush();

  while (true) {
    asm volatile("WFI"  // Wait For Interrupt.
    );
  }
}
#ifdef __cplusplus
}  // extern "C"
#endif
#endif  // CORE_TEENSY

// The tests
#include "tests/add_args.inc"
#include "tests/address.inc"
#include "tests/args.inc"
#include "tests/bundle.inc"
#include "tests/match.inc"
#include "tests/memory.inc"
#include "tests/packet.inc"

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 4000) {
  }
  delay(4000);
}

void loop() {
  Test::run();
}
