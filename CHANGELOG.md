# Changelog for LiteOSCParser

This document details the changes between each release.

## [1.3.0]

### Changed

* Changed parameter types of the private `OSCBundle::add(const uint8_t *, int)`
  to `OSCBundle::add(const uint8_t *, int32_t)`.
* Tested with
  [ArduinoUnit v3.0.2](https://github.com/mmurdoch/arduinounit/releases/tag/v3.0.2).
* On systems where the size of type `double` is not 8 bytes,
  `LiteOSCParser::getDouble` will always return zero and
  `LiteOSCParser::addDouble` will always return `false`.

### Fixed

* Compiles on more platforms (listed using PlatformIO board names): `teensylc`,
  `teensy35`, `teensy36`, `megaatmega1280`, `megaatmega2560`, `uno`, `yun`,
  `featheresp32`, `huzzah`, `esp12e`, `nucleo_f302r8`, `nucleo_f303k8`,
  `nucleo_f103rb`. This is in addition to just compiling and testing on
  `teensy31`.
* Now making use of PlatformIO's `build_unflags` to ensure that the
  `-fsingle-precision-constant` does not take effect; the compiler was
  complaining that some float literals were still overflowing, even though
  `-fno-single-precision-constant` was being passed in `build_flags`.
* Changed `LiteOSCParser::addFloat` and `LiteOSCParser::getFloat` to use
  `static_assert`s to guarantee that the size of type `float` is 4 bytes. This
  ensures that memory is copied into a space having the correct size.


## [1.2.1]

### Added

* keywords.txt file.

## [1.2.0]

### Added

* OSC bundle parsing.

## [1.1.0]

### Added

* OSC bundle support, via a simple `OSCBundle` container class.
* A `LiteOSCParser::clear()` function that empties the message.
* A test for out-of-range matching.
* An all-in-one example.

### Changed

### Removed

* Default copy constructors are removed.

### Fixed

* The constructors now check if the initial memory allocation failed.
* The `match` function now returns the correct value if the offset is equal
  to the address length and the pattern to be matched is empty.

## LiteOSCParser v1.0.0

This is the first LiteOSCParser release. It provides a way to fix the amount
of memory being used internally. Bundles are not currently supported.

---

Copyright (c) 2018-2019 Shawn Silverman
