// OSCBundle.cpp is part of LiteOSCParser.
// (c) 2018 Shawn Silverman

#include "LiteOSCParser.h"

// C++ includes
#include <cstdlib>
#include <cstring>

namespace qindesign {
namespace osc {

OSCBundle::OSCBundle(int bufCapacity)
    : buf_(nullptr),
      bufSize_(0),
      bufCapacity_(0),
      dynamicBuf_(true),
      memoryErr_(false),
      isInitted_(false) {
  if (bufCapacity > 0) {
    dynamicBuf_ = false;
    if (bufCapacity < 16) {
      bufCapacity = 16;
    }
    buf_ = reinterpret_cast<uint8_t*>(malloc(bufCapacity));
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
  return add(bundle.buf_, bundle.size());
}

// --------------------------------------------------------------------------
//  Private functions
// --------------------------------------------------------------------------

bool OSCBundle::add(const uint8_t *buf, int size) {
  if (!isInitted_) {
    return false;
  }
  if (!ensureCapacity(bufSize_ + 4 + size)) {
    return false;
  }

  buf_[bufSize_++] = size >> 24;
  buf_[bufSize_++] = size >> 16;
  buf_[bufSize_++] = size >> 8;
  buf_[bufSize_++] = size;
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
  buf_ = reinterpret_cast<uint8_t*>(realloc(buf_, size));
  if (buf_ == nullptr) {
    memoryErr_ = true;
    return false;
  }
  bufCapacity_ = size;
  return true;
}

}  // namespace osc
}  // namespace qindesign
