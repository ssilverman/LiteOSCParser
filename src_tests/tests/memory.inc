// memory.inc is part of LiteOSCParser.
// (c) 2018 Shawn Silverman

// --------------------------------------------------------------------------
//  Memory tests
// --------------------------------------------------------------------------

test(memory_too_many_args) {
  ::qindesign::osc::LiteOSCParser osc{32, 1};
  const uint8_t b1[12]{ '/', 'a', '\0', 0, ',', 'i', '\0', 0,
                        0x01, 0x02, 0x03, 0x04 };
  assertTrue(osc.parse(b1, sizeof(b1)));
  assertFalse(osc.isMemoryError());

  const uint8_t b2[16]{ '/', 'a', '\0', 0, ',', 'i', 'i', '\0',
                        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  assertFalse(osc.parse(b2, sizeof(b2)));
  assertTrue(osc.isMemoryError());
}

test(memory_dynamic_args) {
  ::qindesign::osc::LiteOSCParser osc{32, 0};
  const uint8_t b1[12]{ '/', 'a', '\0', 0, ',', 'i', '\0', 0,
                        0x01, 0x02, 0x03, 0x04 };
  assertTrue(osc.parse(b1, sizeof(b1)));
  assertFalse(osc.isMemoryError());

  const uint8_t b2[16]{ '/', 'a', '\0', 0, ',', 'i', 'i', '\0',
                        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  assertTrue(osc.parse(b2, sizeof(b2)));
  assertFalse(osc.isMemoryError());
}

test(memory_buf_too_small) {
  ::qindesign::osc::LiteOSCParser osc{8, 3};
  const uint8_t buf[8]{ '/', 'a', '\0', 0, ',', 'T', '\0', 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertFalse(osc.isMemoryError());
  assertEqual(osc.getArgCount(), 1);
  assertEqual(osc.getMessageSize(), 8);

  // Should still have space for one more boolean value
  assertTrue(osc.addBoolean(true));
  assertFalse(osc.isMemoryError());
  assertEqual(osc.getArgCount(), 2);
  assertEqual(osc.getMessageSize(), 8);

  // Now the tags area should expand
  assertFalse(osc.addBoolean(true));
  assertTrue(osc.isMemoryError());
}

test(memory_dynamic_buf) {
  ::qindesign::osc::LiteOSCParser osc{0, 3};
  const uint8_t buf[8]{ '/', 'a', '\0', 0, ',', 'T', '\0', 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertFalse(osc.isMemoryError());
  assertEqual(osc.getArgCount(), 1);
  assertEqual(osc.getMessageSize(), 8);

  // Should still have space for one more boolean value
  assertTrue(osc.addBoolean(true));
  assertFalse(osc.isMemoryError());
  assertEqual(osc.getArgCount(), 2);
  assertEqual(osc.getMessageSize(), 8);

  // Now the tags area should expand
  assertTrue(osc.addBoolean(true));
  assertFalse(osc.isMemoryError());
}

test(memory_small_buf_and_extra_bytes) {
  ::qindesign::osc::LiteOSCParser osc{8, 1};
  uint8_t buf[12]{ '/', 'a', '\0', 0, ',', 'T', '\0', 0, 'x', 0, 0, 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), 8);
}

test(memory_small_buf_and_empty_args) {
  ::qindesign::osc::LiteOSCParser osc{4, 0};
  uint8_t buf[8]{ '/', 'a', '\0', 0, ',', '\0', 0, 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), 4);
}

test(memory_error_then_bad_packet_no_memory_error) {
  ::qindesign::osc::LiteOSCParser osc{4, 1};
  uint8_t buf[8]{ '/', 'a', '\0', 0, ',', 'T', '\0', 0 };
  assertFalse(osc.parse(buf, sizeof(buf)));
  assertTrue(osc.isMemoryError());
  buf[0] = '#';
  assertFalse(osc.parse(buf, sizeof(buf)));
  assertFalse(osc.isMemoryError());
}

test(memory_reused_address_remainder_is_zero) {
  uint8_t buf[8]{ '/', 'a', 'b', 'c', 'd', 'e', 'f', '\0' };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertFalse(osc.isMemoryError());
  assertEqual(osc.getAddress(), "/abcdef");
  assertEqual(osc.getMessageSize(), 8);

  buf[4] = '\0';
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertFalse(osc.isMemoryError());
  assertEqual(osc.getAddress(), "/abc");

  const char b2[8]{ '/', 'a', 'b', 'c', '\0', 0, 0, 0 };
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(b2)));
  for (size_t i = 0; i < sizeof(b2); i++) {
    assertEqual(osc.getMessageBuf()[i], b2[i]);
  }
}
