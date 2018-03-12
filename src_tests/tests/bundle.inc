// bundle.inc is part of LiteOSCParser.
// (c) 2018 Shawn Silverman

// --------------------------------------------------------------------------
//  Bundle tests
// --------------------------------------------------------------------------

test(bundle_buf_too_small) {
  ::qindesign::osc::OSCBundle bundle{16};
  ::qindesign::osc::LiteOSCParser osc;
  osc.init("/a");

  const uint8_t b[16]{ '#', 'b', 'u', 'n', 'd', 'l', 'e', '\0',
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };

  assertTrue(bundle.init(1));
  assertEqual(bundle.size(), 16);
  for (size_t i = 0; i < sizeof(b); i++) {
    assertEqual(bundle.buf()[i], b[i]);
  }

  // Try to add something
  assertFalse(bundle.addMessage(osc));
  assertTrue(bundle.isMemoryError());

  // Assert that the bundle is still the same
  assertEqual(bundle.size(), 16);
  for (size_t i = 0; i < sizeof(b); i++) {
    assertEqual(bundle.buf()[i], b[i]);
  }
}

test(bundle_dynamic_buf) {
  ::qindesign::osc::OSCBundle bundle{0};
  ::qindesign::osc::LiteOSCParser osc;
  osc.init("/a");

  const uint8_t b[24]{ '#', 'b', 'u', 'n', 'd', 'l', 'e', '\0',
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
                       0x00, 0x00, 0x00, 0x04, '/', 'a', '\0', 0 };

  assertTrue(bundle.init(1));
  assertEqual(bundle.size(), 16);
  for (size_t i = 0; i < 16; i++) {
    assertEqual(bundle.buf()[i], b[i]);
  }

  // Try to add something
  assertTrue(bundle.addMessage(osc));
  assertFalse(bundle.isMemoryError());

  // Assert that the bundle is correct
  assertEqual(bundle.size(), static_cast<int>(sizeof(b)));
  for (size_t i = 0; i < sizeof(b); i++) {
    assertEqual(bundle.buf()[i], b[i]);
  }
}

test(bundle_add_message) {
  ::qindesign::osc::OSCBundle bundle;
  ::qindesign::osc::LiteOSCParser osc;
  osc.init("/a");

  const uint8_t b[24]{ '#', 'b', 'u', 'n', 'd', 'l', 'e', '\0',
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
                       0x00, 0x00, 0x00, 0x04, '/', 'a', '\0', 0 };

  assertTrue(bundle.init(1));
  assertEqual(bundle.size(), 16);
  assertTrue(bundle.addMessage(osc));
  assertEqual(bundle.size(), static_cast<int>(sizeof(b)));
  for (size_t i = 0; i < sizeof(b); i++) {
    assertEqual(bundle.buf()[i], b[i]);
  }
}

test(bundle_add_bundle) {
  ::qindesign::osc::OSCBundle bundle;
  ::qindesign::osc::OSCBundle bundle2;
  bundle2.init(2);

  const uint8_t b[36]{ '#', 'b', 'u', 'n', 'd', 'l', 'e', '\0',
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
                       0x00, 0x00, 0x00, 0x10,
                       '#', 'b', 'u', 'n', 'd', 'l', 'e', '\0',
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, };

  assertTrue(bundle.init(1));
  assertEqual(bundle.size(), 16);
  assertTrue(bundle.addBundle(bundle2));
  assertEqual(bundle.size(), static_cast<int>(sizeof(b)));
  for (size_t i = 0; i < sizeof(b); i++) {
    assertEqual(bundle.buf()[i], b[i]);
  }
}

test(bundle_reinit) {
  ::qindesign::osc::OSCBundle bundle;
  ::qindesign::osc::LiteOSCParser osc;
  osc.init("/a");

  assertTrue(bundle.init(1));
  assertEqual(bundle.size(), 16);
  assertTrue(bundle.addMessage(osc));
  assertEqual(bundle.size(), 24);

  const uint8_t b[16]{ '#', 'b', 'u', 'n', 'd', 'l', 'e', '\0',
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 };

  bundle.init(2);
  assertEqual(bundle.size(), static_cast<int>(sizeof(b)));
  for (size_t i = 0; i < sizeof(b); i++) {
    assertEqual(bundle.buf()[i], b[i]);
  }
}
