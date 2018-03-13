#include <LiteOSCParser.h>

::qindesign::osc::LiteOSCParser osc;

const uint8_t goodMessage[]{
    '/', 'a', 'b', 'c', '/', 'd', '\0', 0,
    ',', 'i', '\0', 0, 0x01, 0x02, 0x03, 0x04
};

const uint8_t badMessage[]{
    'a', 'b',  'c', '\0'
};

void setup() {
  while (!Serial && millis() < 2000) {
  }

  Serial.println();
  if (osc.parse(badMessage, sizeof(badMessage))) {
    Serial.println("Bad message: This should not print.");
  } else {
    Serial.println("Bad message: This should print.");
  }

  Serial.println();
  if (osc.parse(goodMessage, sizeof(goodMessage))) {
    Serial.println("Good message: This should print.");
    Serial.printf("  Address:    \"/abc/d\" == \"%s\"\n", osc.getAddress());
    Serial.printf("  Arg count:  1 == %d\n", osc.getArgCount());
    Serial.printf("  Param(0):   'i' == %c'\n", osc.getTag(0));
    Serial.printf("  isFloat(0): false == %s\n",
                  osc.isFloat(0) ? "true" : "false");
    Serial.printf("  isInt(0):   true == %s\n",
                  osc.isInt(0) ? "true" : "false");
    Serial.printf("  getInt(0):  0x01020304 == 0x%08x\n", osc.getInt(0));
    Serial.println("Matching:");
    Serial.printf("  Should fully match \"/abc/d\": true == %s\n",
                  osc.fullMatch(0, "/abc/d") ? "true" : "false");
    Serial.printf("  Should not fully match \"/abc/\": false == %s\n",
                  osc.fullMatch(0, "/abc/") ? "true" : "false");
    Serial.printf("  Should partially match \"/abc\": 4 == %d\n",
                  osc.match(0, "/abc"));
    Serial.printf("  Should not partially match \"/abc/\": 0 == %d\n",
                  osc.match(0, "/abc/"));
    Serial.printf("  Should not partially match \"/ab\": 0 == %d\n",
                  osc.match(0, "/ab"));
    Serial.printf("  Should partially match \"/d\" at 4: 6 == %d\n",
                  osc.match(4, "/d"));
  } else {
    Serial.println("Good message: This should not print.");
  }

  Serial.println();
  Serial.println("Construction:");
  osc.init("/abc/");
  osc.addFloat(1.1f);
  osc.addString("hello");
  Serial.printf("  Address:      \"/abc/\" == \"%s\"\n", osc.getAddress());
  Serial.printf("  Arg count:    2 == %d\n", osc.getArgCount());
  Serial.printf("  getFloat(0):  1.1 == %f\n", osc.getFloat(0));
  Serial.printf("  getString(1): \"hello\" == \"%s\"\n", osc.getString(1));
  Serial.printf("  Should partially match \"/abc\": 4 == %d\n",
                osc.match(0, "/abc"));
}

void loop() {
  // Does nothing.
}
