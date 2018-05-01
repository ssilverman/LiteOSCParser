# Changelog for LiteOSCParser

This document details the changes between each release.

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

Copyright (c) 2018 Shawn Silverman
