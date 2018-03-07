// address.inc is part of OSCParser.
// (c) 2018 Shawn Silverman

// --------------------------------------------------------------------------
//  Address tests
// --------------------------------------------------------------------------

test(address_good_start) {
  const uint8_t buf[4]{ '/', '\0', 0, 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddress(), "/");
  assertEqual(osc.getArgCount(), 0);
}

test(address_invalid) {
  const uint8_t buf[4]{ '/', 'a', 'b', 'c' };
  assertFalse(osc.parse(buf, sizeof(buf)));
}

test(address_valid_2) {
  const uint8_t buf[4]{ '/', 'a', '\0', 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddress(), "/a");
  assertEqual(osc.getArgCount(), 0);
}

test(address_valid_3) {
  const uint8_t buf[4]{ '/', 'a', '/', '\0' };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddress(), "/a/");
  assertEqual(osc.getArgCount(), 0);
}
