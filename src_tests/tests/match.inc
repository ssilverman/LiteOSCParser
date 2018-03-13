// match.inc is part of LiteOSCParser.
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
  assertEqual(osc.getAddress(), "/a");
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
  assertEqual(osc.getAddress(), "/a");
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
  assertEqual(osc.getAddress(), "/a/");
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
  assertEqual(osc.getAddress(), "/ab");
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
  assertEqual(osc.getAddress(), "/a/b");
  assertEqual(osc.getArgCount(), 0);
  assertEqual(osc.match(0, "/a"), 2);
  assertEqual(osc.match(2, "/b"), 4);
}

test(match_offset_out_of_range) {
  const uint8_t buf[8]{'/', 'a', '/', 'b', '\0', 0, 0, 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getAddress(), "/a/b");
  assertEqual(osc.match(-1, "/a"), -1);
  assertEqual(osc.match(4, "/a"), 0);
  assertEqual(osc.match(5, "/b"), -1);
  assertEqual(osc.match(4, ""), 4);
  assertEqual(osc.match(0, ""), 0);
  assertEqual(osc.match(1, ""), 0);
}
