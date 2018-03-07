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
