# Readme for LiteOSCParser v1.3.0

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
[ArduinoUnit](https://github.com/mmurdoch/arduinounit).

Note that the code for ArduinoUnit is not included in this library and needs
to be downloaded separately.

## Notes on use

### Retrieving values

By default, if a value does not exist at a given index, a default value will be
returned. For example, `getFloat(2)` will try to return a 32-bit float value at
index 2. If the index is out of range or if the value type at that index is not
a float then a value of 0.0f will be returned.

To avoid this scenario, the following code is useful:

```c++
if (isFloat(2)) {
  float f = getFloat(2);
  // Do something with the value
}
```

Internally, `getFloat` also calls `isFloat` with the same index, and so
`isFloat` is actually called twice. To avoid two calls, there exist other getter
functions named `getIfXXX` (replacing _XXX_ with the appropriate type). For
example, `getIfFloat` to retrieve a float. These functions return a `bool`:
`true` if the value exists and was retrieved, and `false` otherwise.

For example:

```c++
float f;
if (getIfFloat(2, &f)) {
  // Do something with 'f'
}
```

## Code style

Code style for this project mostly follows the
[Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).

## References

1. [OSC 1.0 Specification](http://opensoundcontrol.org/spec-1_0)
2. [OSC 1.1 Specification](http://opensoundcontrol.org/spec-1_1)
3. [NIME 2009 paper](https://hangar.org/webnou/wp-content/uploads/2012/01/Nime09OSCfinal.pdf)
4. [TouchOSC](https://hexler.net/docs/touchosc)

---

Copyright (c) 2018-2019 Shawn Silverman
