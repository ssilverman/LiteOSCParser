# LiteOSCParser

This library parses and constructs OSC messages. It is designed to minimize
memory usage by providing a way to pre-allocate the internal buffers.

Bundles are supported via a simple container class, `OSCBundle`.

Both classes, `LiteOSCParser` and `OSCBundle`, are in the `qindesign::osc`
namespace.

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
