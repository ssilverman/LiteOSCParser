// packet.inc is part of OSCParser.
// (c) 2018 Shawn Silverman

// --------------------------------------------------------------------------
//  Packet tests
// --------------------------------------------------------------------------

test(packet_odd_length) {
  uint8_t buf[1];
  assertFalse(osc.parse(buf, sizeof(buf)));
}

test(packet_zero_length) {
  uint8_t buf[0];
  assertFalse(osc.parse(buf, sizeof(buf)));
}

test(packet_bad_start) {
  uint8_t buf[4]{ '#', 0, 0, 0 };
  assertFalse(osc.parse(buf, sizeof(buf)));
}

test(packet_disallow_bundle) {
  uint8_t buf[8]{ '#', 'b', 'u', 'n', 'd', 'l', 'e', 0 };
  assertFalse(osc.parse(buf, sizeof(buf)));
}
