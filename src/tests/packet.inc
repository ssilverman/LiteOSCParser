// packet.inc is part of OSCParser.
// (c) 2018 Shawn Silverman

// --------------------------------------------------------------------------
//  Packet tests
// --------------------------------------------------------------------------

test(packet_odd_length) {
  const uint8_t buf[1]{0};
  assertFalse(osc.parse(buf, sizeof(buf)));
}

test(packet_zero_length) {
  const uint8_t buf[0]{};
  assertFalse(osc.parse(buf, sizeof(buf)));
}

test(packet_bad_start) {
  const uint8_t buf[4]{ '#', '\0', 0, 0 };
  assertFalse(osc.parse(buf, sizeof(buf)));
}

test(packet_disallow_bundle) {
  const uint8_t buf[8]{ '#', 'b', 'u', 'n', 'd', 'l', 'e', '\0' };
  assertFalse(osc.parse(buf, sizeof(buf)));
}

test(packet_extra_bytes) {
  const uint8_t buf[8]{ '/', 'a', '\0', 0, 0, 0, 0, 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), 4);
  for (size_t i = 0; i < 4; i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 0);
}

test(packet_args_with_extra_bytes) {
  const uint8_t buf[16]{ '/', 'a', '\0', 0, ',', 'i', '\0', 0,
                        0x01, 0x02, 0x03, 0x04,
                        0, 0, 0, 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), 12);
  for (size_t i = 0; i < 12; i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isInt(0));
  assertEqual(osc.getInt(0), 0x01020304);
}
