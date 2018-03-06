// args.inc is part of OSCParser.
// (c) 2018 Shawn Silverman

// --------------------------------------------------------------------------
//  Args tests
// --------------------------------------------------------------------------

test(args_none) {
  uint8_t buf[4]{ '/', 'a', 0, 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 0);
  assertFalse(osc.isInt(0));
}

test(args_none_with_extra) {
  uint8_t buf[8]{ '/', 'a', 0, 0, 'x', 0, 0, 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 0);
  assertFalse(osc.isInt(0));
}

test(args_none_with_good_start) {
  uint8_t buf[8]{ '/', 'a', 0, 0, ',', 0, 0, 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 0);
  assertFalse(osc.isInt(0));
}

test(args_missing_arg) {
  uint8_t buf[8]{ '/', 'a', 0, 0, ',', 'i', 0, 0 };
  assertFalse(osc.parse(buf, sizeof(buf)));
}

test(args_unknown_arg) {
  uint8_t buf[12]{ '/', 'a', 0, 0, ',', '$', 0, 0, 0, 0, 0, 0 };
  assertFalse(osc.parse(buf, sizeof(buf)));
}

test(args_one_int) {
  uint8_t buf[12]{ '/', 'a', 0, 0, ',', 'i', 0, 0, 0x01, 0x02, 0x03, 0x04 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isInt(0));
  assertFalse(osc.isFloat(0));
  assertEqual(osc.getInt(0), 0x01020304);
  assertEqual(osc.getFloat(0), 0.0f);
}

test(args_one_float) {
  uint8_t buf[12]{ '/', 'a', 0, 0, ',', 'f', 0, 0, 0x47, 0xc3, 0x50, 0x00 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isFloat(0));
  assertFalse(osc.isInt(0));
  assertEqual(osc.getFloat(0), 100000.0f);
  assertEqual(osc.getInt(0), 0);
}

test(args_one_long) {
  uint8_t buf[16]{ '/', 'a', 0, 0, ',', 'h', 0, 0,
                   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isLong(0));
  assertTrue(osc.getLong(0) == 0x0102030405060708LL);
}

test(args_one_time) {
  uint8_t buf[16]{ '/', 'a', 0, 0, ',', 't', 0, 0,
                   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isTime(0));
  assertTrue(osc.getTime(0) == 0x0102030405060708LL);
}

test(args_one_double) {
  uint8_t buf[16]{ '/', 'a', 0, 0, ',', 'd', 0, 0,
                   0x3f, 0xf1, 0x99, 0x99, 0x99, 0x99, 0x99, 0x9a };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isDouble(0));
  assertEqual(osc.getDouble(0), 1.1);
}

test(args_one_true) {
  uint8_t buf[8]{ '/', 'a', 0, 0, ',', 'T', 0, 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isBoolean(0));
  assertEqual(osc.getBoolean(0), true);
}

test(args_one_false) {
  uint8_t buf[8]{ '/', 'a', 0, 0, ',', 'F', 0, 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isBoolean(0));
  assertEqual(osc.getBoolean(0), false);
}

test(args_one_true_one_int) {
  uint8_t buf[12]{ '/', 'a', 0, 0, ',', 'T', 'i', 0, 0x01, 0x02, 0x03, 0x04 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 2);
  assertTrue(osc.isBoolean(0));
  assertEqual(osc.getBoolean(0), true);
  assertTrue(osc.isInt(1));
  assertEqual(osc.getInt(1), 0x01020304);
}

test(args_one_int_one_true) {
  uint8_t buf[12]{ '/', 'a', 0, 0, ',', 'i', 'T', 0, 0x01, 0x02, 0x03, 0x04 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 2);
  assertTrue(osc.isInt(0));
  assertEqual(osc.getInt(0), 0x01020304);
  assertTrue(osc.isBoolean(1));
  assertEqual(osc.getBoolean(1), true);
}
