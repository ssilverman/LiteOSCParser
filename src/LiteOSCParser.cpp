// LiteOSCParser.cpp is part of LiteOSCParser.
// (c) 2018-2019 Shawn Silverman

#include "LiteOSCParser.h"

// C++ includes
#ifdef __has_include
#if __has_include(<cstdlib>)
#include <cstdlib>
#else
#include <stdlib.h>
#endif
#if __has_include(<cstring>)
#include <cstring>
#else
#include <string.h>
#endif
#else
#include <cstdlib>
#include <cstring>
#endif

namespace qindesign {
namespace osc {

LiteOSCParser::LiteOSCParser(int bufCapacity, int maxArgCount)
    : buf_(nullptr),
      bufSize_(0),
      bufCapacity_(0),
      dynamicBuf_(true),
      memoryErr_(false),
      addressLen_(0),
      tagsLen_(0),
      argIndexes_(nullptr),
      argIndexesCapacity_(0),
      dynamicArgIndexes_(true) {
  static_assert(sizeof(uint8_t) == 1, "sizeof(uint8_t) == 1");
  if (bufCapacity > 0) {
    dynamicBuf_ = false;
    buf_ = static_cast<uint8_t*>(malloc(bufCapacity));
    if (buf_ == nullptr) {
      memoryErr_ = true;
    } else {
      bufCapacity_ = bufCapacity;
    }
  }
  if (maxArgCount > 0) {
    dynamicArgIndexes_ = false;
    argIndexes_ = static_cast<int*>(malloc(maxArgCount * sizeof(int)));
    if (argIndexes_ == nullptr) {
      memoryErr_ = true;
    } else {
      argIndexesCapacity_ = maxArgCount;
    }
  }
}

LiteOSCParser::~LiteOSCParser() {
  if (buf_ != nullptr) {
    free(buf_);
  }
  if (argIndexes_ != nullptr) {
    free(argIndexes_);
  }
}

// --------------------------------------------------------------------------
//  Creating
// --------------------------------------------------------------------------

bool LiteOSCParser::init(const char *address) {
  memoryErr_ = false;
  addressLen_ = 0;
  tagsLen_ = 0;
  bufSize_ = 0;

  int addrLen = strlen(address);
  if (addrLen < 1 || address[0] != '/') {
    return false;
  }

  int newSize = align(addrLen + 1);
  if (!ensureCapacity(newSize)) {
    return false;
  }
  strcpy(reinterpret_cast<char*>(buf_), address);
  memset(&buf_[addrLen + 1], 0, newSize - (addrLen + 1));
  addressLen_ = addrLen;
  tagsLen_ = 0;
  tagsIndex_ = newSize;
  dataIndex_ = newSize;
  bufSize_ = newSize;
  return true;
}

bool LiteOSCParser::addInt(int32_t i) {
  if (!addArg('i', 4)) {
    return false;
  }
  setUint(&buf_[bufSize_ - 4], i);
  return true;
}

bool LiteOSCParser::addFloat(float f) {
  if (!addArg('f', 4)) {
    return false;
  }
  uint32_t u;
  memcpy(&u, &f, 4);
  setUint(&buf_[bufSize_ - 4], u);
  return true;
}

bool LiteOSCParser::addString(const char *s) {
  int len = strlen(s) + 1;
  if (!addArg('s', len)) {
    return false;
  }
  strcpy(reinterpret_cast<char*>(&buf_[bufSize_ - align(len)]), s);
  return true;
}

bool LiteOSCParser::addBlob(const uint8_t *b, int len) {
  if (len < 0) {
    return false;
  }
  if (!addArg('b', len + 4)) {
    return false;
  }
  setUint(&buf_[bufSize_ - align(len + 4)], len);
  memcpy(&buf_[bufSize_ - align(len + 4) + 4], b, len);
  return true;
}

bool LiteOSCParser::addLong(int64_t h) {
  if (!addArg('h', 8)) {
    return false;
  }
  setUlong(&buf_[bufSize_ - 8], h);
  return true;
}

bool LiteOSCParser::addTime(uint64_t t) {
  if (!addArg('t', 8)) {
    return false;
  }
  setUlong(&buf_[bufSize_ - 8], t);
  return true;
}

bool LiteOSCParser::addDouble(double d) {
  if (!addArg('d', 8)) {
    return false;
  }
  uint64_t u;
  memcpy(&u, &d, 8);
  setUlong(&buf_[bufSize_ - 8], u);
  return true;
}

bool LiteOSCParser::addBoolean(bool b) {
  return addArg(b ? 'T' : 'F', 0);
}

bool LiteOSCParser::addArg(char tag, int argSize) {
  // Ensure argSize is a multiple of 4
  int newArgSize = align(argSize);
  int tagsSize;  // Includes the NULL
  int newTagsSize;
  if (tagsLen_ == 0) {
    tagsSize = 0;
    newTagsSize = 4;
  } else {
    tagsSize = align(tagsLen_ + 1);  // Include the NULL
    newTagsSize = align(tagsLen_ + 2);  // Plus new tag
  }
  int newSize = bufSize_ + (newTagsSize - tagsSize) + newArgSize;
  if (!ensureCapacity(newSize)) {
    return false;
  }
  // Serial.printf("bufSize=%d newSize=%d tagsSize=%d newTagsSize=%d argSize=%d newArgSize=%d\n",
  //               bufSize_, newSize, tagsSize, newTagsSize, argSize, newArgSize);

  // Possibly allocate more space for the argument indexes
  int newArgCount;
  if (tagsLen_ == 0) {
    newArgCount = 1;
  } else {
    newArgCount = tagsLen_;  // Remember, this includes the ','
  }
  if (!ensureArgIndexesCapacity(newArgCount)) {
    return false;
  }

  // Case where there's no tags already
  if (tagsLen_ == 0) {
    buf_[bufSize_ + 0] = ',';
    buf_[bufSize_ + 1] = tag;
    buf_[bufSize_ + 2] = '\0';
    buf_[bufSize_ + 3] = 0;
    tagsIndex_ = bufSize_;
    tagsLen_ = 2;
    dataIndex_ = tagsIndex_ + 4;
    bufSize_ = newSize;
    argIndexes_[0] = dataIndex_;

    // Now set any extra zeros in the data area
    int delta = newArgSize - argSize;
    memset(&buf_[bufSize_ - delta], 0, delta);

    return true;
  }

  // If the tags are resized then shift everything
  // Remember to include the ',' and the NULL terminators
  int delta = newTagsSize - tagsSize;
  if (delta > 0) {
    memmove(&buf_[dataIndex_ + delta],
            &buf_[dataIndex_],
            bufSize_ - dataIndex_);
    memset(&buf_[tagsIndex_ + tagsSize], 0, delta);
    dataIndex_ += delta;
    bufSize_ += delta;
    for (int i = 0; i < newArgCount - 1; i++) {
      argIndexes_[i] += delta;
    }
  }
  argIndexes_[newArgCount - 1] = bufSize_;

  buf_[tagsIndex_ + tagsLen_] = tag;
  tagsLen_++;
  buf_[tagsIndex_ + tagsLen_] = '\0';

  // Now set any extra zeros in the data area
  delta = newArgSize - argSize;
  memset(&buf_[bufSize_ + argSize], 0, delta);
  bufSize_ += newArgSize;

  return true;
}

// --------------------------------------------------------------------------
//  Parsing and matching
// --------------------------------------------------------------------------

// Notes: Alignment calculation
// Everything will lie on a 4-byte boundary.
// Index mod 4 -> Skip:
// 0 -> 0
// 1 -> 3
// 2 -> 2
// 3 -> 1
// For string lengths, not including the NULL terminator:
// 0 -> 3
// 1 -> 2
// 2 -> 1
// 3 -> 0

bool LiteOSCParser::parse(const uint8_t *buf, int len) {
  memoryErr_ = false;
  addressLen_ = 0;
  tagsLen_ = 0;
  bufSize_ = 0;

  if ((len & 0x03) != 0 || len <= 0) {
    return false;
  }

  // Address
  if (buf[0] != '/') {
    return false;
  }
  // Address is at index 0
  int index = parseString(buf, 0, len);
  if (index < 0) {
    return false;
  }
  addressLen_ = index - 1;
  index = align(index);

  // Type tags

  // No tags
  if (index >= len || buf[index] != ',') {
    if (!ensureCapacity(index)) {
      addressLen_ = 0;
      tagsLen_ = 0;
      return false;
    }
    memcpy(buf_, buf, index);
    memset(&buf_[addressLen_ + 1], 0, index - (addressLen_ + 1));

    tagsIndex_ = index;
    tagsLen_ = 0;
    dataIndex_ = index;
    bufSize_ = index;
    return true;
  }

  tagsIndex_ = index;
  index = parseString(buf, index, len);
  if (index < 0) {
    addressLen_ = 0;
    tagsLen_ = 0;
    return false;
  }
  tagsLen_ = index - 1 - tagsIndex_;
  if (tagsLen_ == 1) {
    if (!ensureCapacity(tagsIndex_)) {
      addressLen_ = 0;
      tagsLen_ = 0;
      return false;
    }
    memcpy(buf_, buf, tagsIndex_);
    memset(&buf_[addressLen_ + 1], 0, tagsIndex_ - (addressLen_ + 1));

    tagsLen_ = 0;
    dataIndex_ = tagsIndex_;
    bufSize_ = tagsIndex_;
    return true;
  }
  index = align(index);

  // Args
  if (!ensureArgIndexesCapacity(tagsLen_ - 1)) {
    addressLen_ = 0;
    tagsLen_ = 0;
    return false;
  }
  dataIndex_ = index;
  index = parseArgs(buf, index, len);
  if (index < 0) {
    addressLen_ = 0;
    tagsLen_ = 0;
    return false;
  }

  if (!ensureCapacity(index)) {
    addressLen_ = 0;
    tagsLen_ = 0;
    return false;
  }
  memcpy(buf_, buf, index);
  memset(&buf_[addressLen_ + 1], 0, tagsIndex_ - (addressLen_ + 1));
  memset(&buf_[tagsIndex_ + tagsLen_ + 1], 0,
         dataIndex_ - (tagsIndex_ + tagsLen_ + 1));
  bufSize_ = index;

  return true;
}

bool LiteOSCParser::fullMatch(int offset, const char *pattern) const {
  if (offset < 0 || addressLen_ < offset) {
    return false;
  }
  if (offset == addressLen_) {
    return strlen(pattern) == 0;
  }
  return strcmp(reinterpret_cast<char*>(&buf_[offset]), pattern) == 0;
}

int LiteOSCParser::match(int offset, const char *pattern) const {
  if (offset < 0 || addressLen_ < offset) {
    return -1;
  }
  if (offset == addressLen_) {
    if (*pattern == '\0') {
      return offset;
    }
    return 0;
  }

  int loc;
  if (strcmploc(reinterpret_cast<char*>(&buf_[offset]), pattern, &loc)) {
    return addressLen_;
  }
  loc += offset;

  // There's a match if the next character is a '/'
  // Strictly speaking, we don't need to check if loc is in range
  // because one past the end is the NULL terminator, and that's
  // valid memory
  if (buf_[loc] == '/') {
    return loc;
  }

  return 0;
}

// Similar to:
// https://github.com/ARM-software/arm-trusted-firmware/blob/master/lib/stdlib/strcmp.c
bool LiteOSCParser::strcmploc(const char *s1, const char *s2, int *loc) {
  int count = 0;
  while (*s1 == *(s2++)) {
    if (*(s1++) == '\0') {
      return true;
    }
    count++;
  }
  *loc = count;
  return false;
}

// --------------------------------------------------------------------------
//  Getters
// --------------------------------------------------------------------------

const char *LiteOSCParser::getAddress() const {
  return reinterpret_cast<char*>(&buf_[0]);
}

int32_t LiteOSCParser::getInt(int index) const {
  if (!isInt(index)) {
    return 0;
  }
  return getUint(&buf_[argIndexes_[index]]);
}

float LiteOSCParser::getFloat(int index) const {
  if (!isFloat(index)) {
    return 0.0f;
  }
  uint32_t u = getUint(&buf_[argIndexes_[index]]);
  float f;
  memcpy(&f, &u, 4);
  return f;
}

const char *LiteOSCParser::getString(int index) const {
  if (!isString(index)) {
    return nullptr;
  }
  return reinterpret_cast<char*>(&buf_[argIndexes_[index]]);
}

int LiteOSCParser::getBlobLength(int index) const {
  if (!isBlob(index)) {
    return 0;
  }
  int32_t size = getUint(&buf_[argIndexes_[index]]);
  if (size < 0) {
    size = 0;
  }
  return size;
}

const uint8_t *LiteOSCParser::getBlob(int index) const {
  if (!isBlob(index)) {
    return nullptr;
  }
  return &buf_[argIndexes_[index] + 4];
}

int64_t LiteOSCParser::getLong(int index) const {
  if (!isLong(index)) {
    return 0;
  }
  return getUlong(&buf_[argIndexes_[index]]);
}

uint64_t LiteOSCParser::getTime(int index) const {
  if (!isTime(index)) {
    return 0;
  }
  return getUlong(&buf_[argIndexes_[index]]);
}

double LiteOSCParser::getDouble(int index) const {
  if (!isDouble(index)) {
    return 0.0;
  }
  int64_t u = getUlong(&buf_[argIndexes_[index]]);
  double d;
  memcpy(&d, &u, 8);
  return d;
}

int32_t LiteOSCParser::getChar(int index) const {
  if (!isChar(index)) {
    return 0;
  }
  return getUint(&buf_[argIndexes_[index]]);
}

bool LiteOSCParser::getBoolean(int index) const {
  if (!isBoolean(index)) {
    return false;
  }
  return buf_[tagsIndex_ + 1 + index] == 'T';
}

// --------------------------------------------------------------------------
//  Private functions
// --------------------------------------------------------------------------

bool LiteOSCParser::ensureCapacity(int size) {
  // // Ensure there's a multiple of 4 bytes
  // size = ((size + 3)>>2)<<2;

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

bool LiteOSCParser::ensureArgIndexesCapacity(int size) {
  if (size <= argIndexesCapacity_) {
    return true;
  }
  if (!dynamicArgIndexes_) {
    memoryErr_ = true;
    return false;
  }
  argIndexes_ =
      static_cast<int*>(realloc(argIndexes_, size * sizeof(int)));
  if (argIndexes_ == nullptr) {
    memoryErr_ = true;
    return false;
  }
  argIndexesCapacity_ = size;
  return true;
}

int LiteOSCParser::parseString(const uint8_t *buf, int off, int len) {
  while (buf[off++] != 0) {
    if (off >= len) {
      return -1;
    }
  }
  return off;
}

int LiteOSCParser::parseArgs(const uint8_t *buf, int off, int len) {
  for (int i = 0; i < tagsLen_ - 1; i++) {
    argIndexes_[i] = off;
    switch (buf[i + tagsIndex_ + 1]) {
      case 'i':  // int32
      case 'f':  // float32
      case 'c':  // char
      case 'r':  // RGBA
      case 'm':  // MIDI
        off += 4;
        break;

      case 'h':  // int64
      case 't':  // timetag
      case 'd':  // float64
        off += 8;
        break;

      case 's':  // OSC-string
      case 'S':  // symbol, same as string
        off = parseString(buf, off, len);
        if (off < 0) {
          return -1;
        }
        off = align(off);
        break;

      case 'b': {  // OSC-blob
        if (off + 4 > len) {
          return -1;
        }
        int32_t size = getUint(&buf[off]);
        if (size < 0) {
          return -1;
        }
        off = align(off + 4 + size);
        break;
      }

      case 'T':  // True
      case 'F':  // False
      case 'N':  // Nil
      case 'I':  // Infinitum
      case '[':  // Array begin
      case ']':  // Array end
        break;

      default:
        return -1;
    }
    if (off > len) {
      return -1;
    }
  }
  return off;
}

}  // namespace osc
}  // namespace qindesign
