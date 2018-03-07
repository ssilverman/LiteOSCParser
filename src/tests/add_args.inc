// add_args.inc is part of OSCParser.
// (c) 2018 Shawn Silverman

// --------------------------------------------------------------------------
//  Adding args tests
// --------------------------------------------------------------------------

test(add_first_int) {
  const uint8_t buf[4]{ '/', 'a', '\0', 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 0);

  assertTrue(osc.addInt(0x01020304));
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isInt(0));
  assertEqual(osc.getInt(0), 0x01020304);

  char b2[12]{ '/', 'a', '\0', 0, ',', 'i', '\0', 0,
               0x01, 0x02, 0x03, 0x04 };
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(b2)));
  for (size_t i = 0; i < sizeof(b2); i++) {
    assertEqual(osc.getMessageBuf()[i], b2[i]);
  }
}

test(add_int_to_empty_tags) {
  const uint8_t buf[8]{ '/', 'a', '\0', 0, ',', '\0', 0, 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), 4);
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 0);

  assertTrue(osc.addInt(0x01020304));
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isInt(0));
  assertEqual(osc.getInt(0), 0x01020304);

  char b2[12]{ '/', 'a', '\0', 0, ',', 'i', '\0', 0,
               0x01, 0x02, 0x03, 0x04 };
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(b2)));
  for (size_t i = 0; i < sizeof(b2); i++) {
    assertEqual(osc.getMessageBuf()[i], b2[i]);
  }
}

test(add_second_int) {
  const uint8_t buf[12]{ '/', 'a', '\0', 0, ',', 'i', '\0', 0,
                         0x01, 0x02, 0x03, 0x04 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isInt(0));
  assertEqual(osc.getInt(0), 0x01020304);

  assertTrue(osc.addInt(0x05060708));
  assertEqual(osc.getArgCount(), 2);
  assertTrue(osc.isInt(1));
  assertEqual(osc.getInt(1), 0x05060708);

  char b2[16]{ '/', 'a', '\0', 0, ',', 'i', 'i', '\0',
               0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(b2)));
  for (size_t i = 0; i < sizeof(b2); i++) {
    assertEqual(osc.getMessageBuf()[i], b2[i]);
  }
}

test(add_int_to_full) {
  const uint8_t buf[16]{ '/', 'a', '\0', 0, ',', 'i', 'i', '\0',
                         0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 2);
  assertTrue(osc.isInt(0));
  assertEqual(osc.getInt(0), 0x01020304);
  assertTrue(osc.isInt(1));
  assertEqual(osc.getInt(1), 0x05060708);

  assertTrue(osc.addInt(0x090a0b0c));
  assertEqual(osc.getArgCount(), 3);
  assertTrue(osc.isInt(2));
  assertEqual(osc.getInt(2), 0x090a0b0c);

  char b2[24]{ '/', 'a', '\0', 0,
               ',', 'i', 'i', 'i', '\0', 0, 0, 0,
               0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
               0x09, 0x0a, 0x0b, 0x0c };
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(b2)));
  for (size_t i = 0; i < sizeof(b2); i++) {
    assertEqual(osc.getMessageBuf()[i], b2[i]);
  }
}

test(add_first_float) {
  const uint8_t buf[4]{ '/', 'a', '\0', 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 0);

  assertTrue(osc.addFloat(-1.1f));
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isFloat(0));
  assertEqual(osc.getFloat(0), -1.1f);

  char b2[12]{ '/', 'a', '\0', 0, ',', 'f', '\0', 0,
               0xbf, 0x8c, 0xcc, 0xcd };
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(b2)));
  for (size_t i = 0; i < sizeof(b2); i++) {
    assertEqual(osc.getMessageBuf()[i], b2[i]);
  }
}

test(add_first_long) {
  const uint8_t buf[4]{ '/', 'a', '\0', 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 0);

  assertTrue(osc.addLong(0x0102030405060708LL));
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isLong(0));
  assertTrue(osc.getLong(0) == 0x0102030405060708LL);

  char b2[16]{ '/', 'a', '\0', 0, ',', 'h', '\0', 0,
               0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(b2)));
  for (size_t i = 0; i < sizeof(b2); i++) {
    assertEqual(osc.getMessageBuf()[i], b2[i]);
  }
}

test(add_first_time) {
  const uint8_t buf[4]{ '/', 'a', '\0', 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 0);

  assertTrue(osc.addTime(0x0102030405060708LL));
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isTime(0));
  assertTrue(osc.getTime(0) == 0x0102030405060708LL);

  char b2[16]{ '/', 'a', '\0', 0, ',', 't', '\0', 0,
               0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(b2)));
  for (size_t i = 0; i < sizeof(b2); i++) {
    assertEqual(osc.getMessageBuf()[i], b2[i]);
  }
}

test(add_first_double) {
  const uint8_t buf[4]{ '/', 'a', '\0', 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 0);

  assertTrue(osc.addDouble(4.1));
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isDouble(0));
  assertEqual(osc.getDouble(0), 4.1);

  char b2[16]{ '/', 'a', '\0', 0, ',', 'd', '\0', 0,
               0x40, 0x10, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66 };
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(b2)));
  for (size_t i = 0; i < sizeof(b2); i++) {
    assertEqual(osc.getMessageBuf()[i], b2[i]);
  }
}

test(add_first_boolean_true) {
  const uint8_t buf[4]{ '/', 'a', '\0', 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 0);

  assertTrue(osc.addBoolean(true));
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isBoolean(0));
  assertEqual(osc.getBoolean(0), true);

  char b2[8]{ '/', 'a', '\0', 0, ',', 'T', '\0', 0 };
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(b2)));
  for (size_t i = 0; i < sizeof(b2); i++) {
    assertEqual(osc.getMessageBuf()[i], b2[i]);
  }
}

test(add_first_boolean_false) {
  const uint8_t buf[4]{ '/', 'a', '\0', 0 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 0);

  assertTrue(osc.addBoolean(false));
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isBoolean(0));
  assertEqual(osc.getBoolean(0), false);

  char b2[8]{ '/', 'a', '\0', 0, ',', 'F', '\0', 0 };
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(b2)));
  for (size_t i = 0; i < sizeof(b2); i++) {
    assertEqual(osc.getMessageBuf()[i], b2[i]);
  }
}

test(add_boolean_second) {
  const uint8_t buf[12]{ '/', 'a', '\0', 0, ',', 'i', '\0', 0,
                         0x01, 0x02, 0x03, 0x04 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 1);
  assertTrue(osc.isInt(0));
  assertEqual(osc.getInt(0), 0x01020304);

  assertTrue(osc.addBoolean(true));
  assertEqual(osc.getArgCount(), 2);
  assertTrue(osc.isBoolean(1));
  assertEqual(osc.getBoolean(1), true);

  char b2[12]{ '/', 'a', '\0', 0, ',', 'i', 'T', '\0',
               0x01, 0x02, 0x03, 0x04 };
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(b2)));
  for (size_t i = 0; i < sizeof(b2); i++) {
    assertEqual(osc.getMessageBuf()[i], b2[i]);
  }
}

test(add_boolean_to_full) {
  const uint8_t buf[16]{ '/', 'a', '\0', 0, ',', 'i', 'i', '\0',
                         0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  assertTrue(osc.parse(buf, sizeof(buf)));
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); i++) {
    assertEqual(osc.getMessageBuf()[i], buf[i]);
  }
  assertEqual(osc.getAddressLength(), 2);
  assertEqual(osc.getArgCount(), 2);
  assertTrue(osc.isInt(0));
  assertEqual(osc.getInt(0), 0x01020304);
  assertTrue(osc.isInt(1));
  assertEqual(osc.getInt(1), 0x05060708);

  assertTrue(osc.addBoolean(true));
  assertEqual(osc.getArgCount(), 3);
  assertTrue(osc.isBoolean(2));
  assertEqual(osc.getBoolean(2), true);

  char b2[20]{ '/', 'a', '\0', 0,
               ',', 'i', 'i', 'T', '\0', 0, 0, 0,
               0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  assertEqual(osc.getMessageSize(), static_cast<int>(sizeof(b2)));
  for (size_t i = 0; i < sizeof(b2); i++) {
    assertEqual(osc.getMessageBuf()[i], b2[i]);
  }
}
