// match.inc is part of OSCParser.
// (c) 2018 Shawn Silverman

// --------------------------------------------------------------------------
//  Match tests
// --------------------------------------------------------------------------

test(match_full_exact) {
  const uint8_t buf[4]{'/', 'a', '\0', 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 0);
  assertTrue(osc.fullMatch(0, "/a"));
  assertTrue(osc.fullMatch(0, "/a"));
  assertFalse(osc.fullMatch(1, "/a"));
}

test(match_partial_complete) {
  const uint8_t buf[4]{'/', 'a', '\0', 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 0);
  assertEqual(osc.match(0, "/a"), 2);
}

test(match_partial) {
  const uint8_t buf[4]{'/', 'a', '/', '\0' };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddressLength(), 3);
  assertEqual(osc.getArgCount(), 0);
  assertEqual(osc.match(0, "/a"), 2);
}

test(match_partial_mismatch) {
  const uint8_t buf[4]{'/', 'a', 'b', '\0' };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddressLength(), 3);
  assertEqual(osc.getArgCount(), 0);
  assertEqual(osc.match(0, "/a"), 0);
}

test(match_partial_twice) {
  const uint8_t buf[8]{'/', 'a', '/', 'b', '\0', 0, 0, 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddressLength(), 4);
  assertEqual(osc.getArgCount(), 0);
  assertEqual(osc.match(0, "/a"), 2);
  assertEqual(osc.match(2, "/b"), 4);
}
