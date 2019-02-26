// OSCBundle.cpp is part of LiteOSCParser.
// (c) 2018 Shawn Silverman

#include "LiteOSCParser.h"

// C++ includes
#if defined(ESP8266)
#include <cstdlib>
#else
#if __has_include(<cstdlib>)
#include <cstdlib>
#else
#include <stdlib.h>
#endif
#endif

#if defined(ESP8266)
#include <cstring>
#else
#if __has_include(<cstring>)
#include <cstring>
#else
#include <string.h>
#endif
#endif

namespace qindesign {
namespace osc {

OSCBundle::OSCBundle(int bufCapacity)
    : buf_(nullptr),
      bufSize_(0),
      bufCapacity_(0),
      dynamicBuf_(true),
      memoryErr_(false),
      isInitted_(false) {
  static_assert(sizeof(uint8_t) == 1, "sizeof(uint8_t) != 1");
  if (bufCapacity > 0) {
    dynamicBuf_ = false;
    if (bufCapacity < 16) {
      bufCapacity = 16;
    }
    buf_ = static_cast<uint8_t*>(malloc(bufCapacity));
    if (buf_ == nullptr) {
      memoryErr_ = true;
    } else {
      bufCapacity_ = bufCapacity;
    }
  }
}

OSCBundle::~OSCBundle() {
  if (buf_ != nullptr) {
    free(buf_);
  }
}

bool OSCBundle::init(uint64_t time) {
  if (!ensureCapacity(16)) {
    return false;
  }
  memcpy(buf_, "#bundle", 8);

  // Use 32-bit values
  uint32_t t = time >> 32;
  buf_[8] = t >> 24;
  buf_[9] = t >> 16;
  buf_[10] = t >> 8;
  buf_[11] = t;

  t = time;
  buf_[12] = t >> 24;
  buf_[13] = t >> 16;
  buf_[14] = t >> 8;
  buf_[15] = t;

  bufSize_ = 16;

  isInitted_ = true;
  return true;
}

bool OSCBundle::addMessage(const LiteOSCParser &osc) {
  return add(osc.getMessageBuf(), osc.getMessageSize());
}

bool OSCBundle::addBundle(const OSCBundle &bundle) {
  return add(bundle.buf(), bundle.size());
}

bool OSCBundle::parse(const uint8_t *buf, int32_t len) {
  if (len < 16 || (len & 0x03) != 0) {
    return false;
  }
  if (memcmp(buf, "#bundle", 8) != 0) {
    return false;
  }
  int index = 16;
  while (index < len) {
    int32_t size = static_cast<int32_t>(
        uint32_t{buf[index]} << 24 | uint32_t{buf[index + 1]} << 16 |
        uint32_t{buf[index + 2]} << 8 | uint32_t{buf[index + 3]});
    index += 4;
    if (size <= 0 || (size & 0x03) != 0 || index + size > len) {
      return false;
    }
    if (size >= 8 && memcmp(&buf[index], "#bundle", 8) == 0) {
      if (!parse(&buf[index], size)) {
        return false;
      }
    } else if (buf[index] != '/') {  // Rudimentary check for an OSC message
      return false;
    }
    index += size;
  }
  return true;
}

// --------------------------------------------------------------------------
//  Private functions
// --------------------------------------------------------------------------

bool OSCBundle::add(const uint8_t *buf, int32_t size) {
  if (!isInitted_) {
    return false;
  }
  if (!ensureCapacity(bufSize_ + 4 + size)) {
    return false;
  }

  uint32_t u = static_cast<uint32_t>(size);
  buf_[bufSize_++] = u >> 24;
  buf_[bufSize_++] = u >> 16;
  buf_[bufSize_++] = u >> 8;
  buf_[bufSize_++] = u;
  memcpy(&buf_[bufSize_], buf, size);
  bufSize_ += size;

  return true;
}

bool OSCBundle::ensureCapacity(int size) {
  if (size <= bufCapacity_) {
    return true;
  }
  if (!dynamicBuf_) {
    memoryErr_ = true;
    return false;
  }
  buf_ = static_cast<uint8_t*>(realloc(buf_, size));
  if (buf_ == nullptr) {
    memoryErr_ = true;
    return false;
  }
  bufCapacity_ = size;
  return true;
}

}  // namespace osc
}  // namespace qindesign
