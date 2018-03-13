# LiteOSCParser

This library parses and constructs OSC messages. It is designed to minimize
memory usage by providing a way to pre-allocate the internal buffers.

Bundles are supported via a simple container class.

## Features

Notable features of this library:

1. Ability to control memory usage.
2. Lightweight API.

## How to use

The classes you'll need are in the `qindesign::osc` namespace:
`LiteOSCParser` and possibly `OSCBundle`. A complete example of how to use
the parser is in the `AllInOne` example.

The main class documentation can be found in `src/LiteOSCParser.h`.

OSC bundles are managed using the simple `OSCBundle` container class.

## Installing as an Arduino library

Not all the files in this project are necessary in an installed library.
Only the following files and directories need to be there:

* `*.md`
* `LICENSE`
* `examples/`
* `library.json`
* `library.properties`
* `src/`

## Running the tests

There are tests included in this project that rely on a project called
[ArduinoUnit](https://github.com/mmurdoch/arduinounit). This should work
out of the box on a Teensy, but some modifications may need to be made for
ESP8266-based devices.

Note that the code for ArduinoUnit is not included in this library and needs
to be downloaded separately.

## Code style

Code style for this project follows the
[Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).

## References

1. [OSC 1.0 Specification](http://opensoundcontrol.org/spec-1_0)
2. [OSC 1.1 Specification](http://opensoundcontrol.org/spec-1_1)
3. [NIME 2009 paper](https://hangar.org/webnou/wp-content/uploads/2012/01/Nime09OSCfinal.pdf)
4. [TouchOSC](https://hexler.net/docs/touchosc)

---

Copyright (c) 2018 Shawn Silverman
