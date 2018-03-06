// address.inc is part of OSCParser.
// (c) 2018 Shawn Silverman

// --------------------------------------------------------------------------
//  Address tests
// --------------------------------------------------------------------------

test(address_good_start) {
  uint8_t buf[4]{ '/', 0, 0, 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getAddressLength(), 1);
  assertEqual(osc.getArgCount(), 0);
}

test(address_invalid) {
  uint8_t buf[4]{ '/', 'a', 'b', 'c' };
  assertFalse(osc.parse(buf, sizeof(buf)));
}

test(address_valid_2) {
  uint8_t buf[4]{ '/', 'a', 0, 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 0);
}

test(address_valid_3) {
  uint8_t buf[4]{ '/', 'a', '/', 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getAddressLength(), 3);
  assertEqual(osc.getArgCount(), 0);
}
