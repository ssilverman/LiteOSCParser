// args.inc is part of LiteOSCParser.
// (c) 2018 Shawn Silverman

// --------------------------------------------------------------------------
//  Args tests
// --------------------------------------------------------------------------

test(args_none) {
  const uint8_t buf[4]{ '/', 'a', '\0', 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddress(), "/a");
  assertEqual(osc.getArgCount(), 0);
  assertFalse(osc.isInt(0));
}

test(args_none_with_extra) {
  const uint8_t buf[8]{ '/', 'a', '\0', 0, 'x', 0, 0, 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), 4);
  for (size_t i = 0; i < 4; i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddress(), "/a");
  assertEqual(osc.getArgCount(), 0);
  assertFalse(osc.isInt(0));
}

test(args_none_with_good_start) {
  const uint8_t buf[8]{ '/', 'a', '\0', 0, ',', '\0', 0, 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), 4);
  for (size_t i = 0; i < 4; i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddress(), "/a");
  assertEqual(osc.getArgCount(), 0);
  assertFalse(osc.isInt(0));
}

test(args_missing_arg) {
  const uint8_t buf[8]{ '/', 'a', '\0', 0, ',', 'i', '\0', 0 };
  assertFalse(osc.parse(buf, sizeof(buf)));
}

test(args_unknown_arg) {
  const uint8_t buf[12]{ '/', 'a', '\0', 0, ',', '$', '\0', 0, 0, 0, 0, 0 };
  assertFalse(osc.parse(buf, sizeof(buf)));
}

test(args_one_int) {
  const uint8_t buf[12]{ '/', 'a', '\0', 0, ',', 'i', '\0', 0,
                         0x01, 0x02, 0x03, 0x04 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddress(), "/a");
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isInt(0));
  assertFalse(osc.isFloat(0));
  assertEqual(osc.getInt(0), 0x01020304);
  assertEqual(osc.getFloat(0), 0.0f);
}

test(args_one_float) {
  const uint8_t buf[12]{ '/', 'a', '\0', 0, ',', 'f', '\0', 0,
                         0x47, 0xc3, 0x50, 0x00 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddress(), "/a");
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isFloat(0));
  assertFalse(osc.isInt(0));
  assertEqual(osc.getFloat(0), 100000.0f);
  assertEqual(osc.getInt(0), 0);
}

test(args_one_long) {
  const uint8_t buf[16]{ '/', 'a', '\0', 0, ',', 'h', '\0', 0,
                         0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddress(), "/a");
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isLong(0));
  assertTrue(osc.getLong(0) == 0x0102030405060708LL);
}

test(args_one_time) {
  const uint8_t buf[16]{ '/', 'a', '\0', 0, ',', 't', '\0', 0,
                         0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddress(), "/a");
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isTime(0));
  assertTrue(osc.getTime(0) == 0x0102030405060708LL);
}

test(args_one_double) {
  const uint8_t buf[16]{ '/', 'a', '\0', 0, ',', 'd', '\0', 0,
                         0x3f, 0xf1, 0x99, 0x99, 0x99, 0x99, 0x99, 0x9a };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddress(), "/a");
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isDouble(0));
  assertEqual(osc.getDouble(0), 1.1);
}

test(args_one_char) {
  const uint8_t buf[12]{ '/', 'a', '\0', 0, ',', 'c', '\0', 0,
                         0x00, 0x00, 0x00, 0x61 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddress(), "/a");
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isChar(0));
  assertEqual(osc.getChar(0), 0x61);
}

test(args_one_true) {
  const uint8_t buf[8]{ '/', 'a', '\0', 0, ',', 'T', '\0', 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddress(), "/a");
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isBoolean(0));
  assertEqual(osc.getBoolean(0), true);
}

test(args_one_false) {
  const uint8_t buf[8]{ '/', 'a', '\0', 0, ',', 'F', '\0', 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddress(), "/a");
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isBoolean(0));
  assertEqual(osc.getBoolean(0), false);
}

test(args_one_true_one_int) {
  const uint8_t buf[12]{ '/', 'a', '\0', 0, ',', 'T', 'i', '\0',
                         0x01, 0x02, 0x03, 0x04 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddress(), "/a");
  assertEqual(osc.getArgCount(), 2);
  assertTrue(osc.isBoolean(0));
  assertEqual(osc.getBoolean(0), true);
  assertTrue(osc.isInt(1));
  assertEqual(osc.getInt(1), 0x01020304);
}

test(args_one_int_one_true) {
  const uint8_t buf[12]{ '/', 'a', '\0', 0, ',', 'i', 'T', '\0',
                         0x01, 0x02, 0x03, 0x04 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddress(), "/a");
  assertEqual(osc.getArgCount(), 2);
  assertTrue(osc.isInt(0));
  assertEqual(osc.getInt(0), 0x01020304);
  assertTrue(osc.isBoolean(1));
  assertEqual(osc.getBoolean(1), true);
}

test(args_one_string) {
  const uint8_t buf[12]{ '/', 'a', '\0', 0, ',', 's', '\0', 0,
                         'h', 'i', '!', '\0' };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddress(), "/a");
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isString(0));
  assertEqual(osc.getString(0), "hi!");
}

test(args_one_blob) {
  const uint8_t buf[16]{ '/', 'a', '\0', 0, ',', 'b', '\0', 0,
                         0x00, 0x00, 0x00, 0x04,
                         'h', 'i', '!', '\0' };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddress(), "/a");
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isBlob(0));
  assertEqual(osc.getBlobLength(0), 4);
  assertEqual(reinterpret_cast<const char*>(osc.getBlob(0)), "hi!");
}

test(args_one_null) {
  const uint8_t buf[8]{ '/', 'a', '\0', 0, ',', 'N', '\0', 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddress(), "/a");
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isNull(0));
}

test(args_one_impulse) {
  const uint8_t buf[8]{ '/', 'a', '\0', 0, ',', 'I', '\0', 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddress(), "/a");
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isImpulse(0));
}
