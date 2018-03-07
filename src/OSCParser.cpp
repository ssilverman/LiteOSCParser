// OSCParser.cpp is part of LiteOSCParser.
// (c) 2018 Shawn Silverman

#include "OSCParser.h"

// C++ includes
#include <cstdlib>
#include <cstring>

// Other includes
#include <Arduino.h>

namespace qindesign {
namespace osc {

OSCParser::OSCParser(int bufCapacity, int maxArgCount)
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
  if (bufCapacity > 0) {
    dynamicBuf_ = false;
    bufCapacity_ = bufCapacity;
    buf_ = reinterpret_cast<uint8_t*>(malloc(bufCapacity));
  }
  if (maxArgCount > 0) {
    dynamicArgIndexes_ = false;
    argIndexesCapacity_ = maxArgCount;
    argIndexes_ = reinterpret_cast<int*>(malloc(maxArgCount * sizeof(int)));
  }
}

OSCParser::~OSCParser() {
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

bool OSCParser::init(const char *address) {
  memoryErr_ = false;

  int addrLen = strlen(address);
  if (addrLen < 1 || address[0] != '/') {
    return false;
  }

  int newSize = align(addrLen + 1);
  if (!ensureCapacity(newSize)) {
    return false;
  }
  strcpy(reinterpret_cast<char*>(buf_), address);
  addressLen_ = addrLen;
  tagsLen_ = 0;
  bufSize_ = newSize;
  return true;
}

bool OSCParser::addInt(int32_t i) {
  if (!addArg('i', 4)) {
    return false;
  }
  setInt(&buf_[bufSize_ - 4], i);
  return true;
}

bool OSCParser::addFloat(float f) {
  if (!addArg('f', 4)) {
    return false;
  }
  int32_t i;
  memcpy(&i, &f, 4);
  setInt(&buf_[bufSize_ - 4], i);
  return true;
}

bool OSCParser::addLong(int64_t h) {
  if (!addArg('h', 8)) {
    return false;
  }
  setLong(&buf_[bufSize_ - 8], h);
  return true;
}

bool OSCParser::addTime(int64_t t) {
  if (!addArg('t', 8)) {
    return false;
  }
  setLong(&buf_[bufSize_ - 8], t);
  return true;
}

bool OSCParser::addDouble(double d) {
  if (!addArg('d', 8)) {
    return false;
  }
  int64_t i;
  memcpy(&i, &d, 8);
  setLong(&buf_[bufSize_ - 8], i);
  return true;
}

bool OSCParser::addBoolean(bool b) {
  return addArg(b ? 'T' : 'F', 0);
}

bool OSCParser::addString(const char *s) {
  int len = strlen(s) + 1;
  if (!addArg('s', len)) {
    return false;
  }
  strcpy(reinterpret_cast<char*>(&buf_[bufSize_ - align(len)]), s);
  return true;
}

bool OSCParser::addBlob(const uint8_t *b, int len) {
  if (len < 0) {
    return false;
  }
  if (!addArg('b', len + 4)) {
    return false;
  }
  setInt(&buf_[bufSize_ - align(len + 4)], len);
  memcpy(&buf_[bufSize_ - align(len + 4) + 4], b, len);
  return true;
}

bool OSCParser::addArg(char tag, int argSize) {
  // Ensure argSize is a multiple of 4
  int newArgSize = align(argSize);
  int tagsSize;
  int newTagsSize;
  if (tagsLen_ == 0) {
    tagsSize = 0;
    newTagsSize = 4;
  } else {
    tagsSize = align(tagsLen_ + 1);  // Includes the NULL
    newTagsSize = align(tagsLen_ + 2);  // Plus new tag
  }
  int newSize = bufSize_ + (newTagsSize - tagsSize) +  newArgSize;
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

bool OSCParser::parse(const uint8_t *buf, int len) {
  memoryErr_ = false;
  addressLen_ = 0;
  tagsLen_ = 0;

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

  // Copy everything into our local buffer
  if (!ensureCapacity(len)) {
    return false;
  }
  memcpy(buf_, buf, len);

  // Type tags

  // No tags
  if (index >= len || buf[index] != ',') {
    tagsIndex_ = index;
    tagsLen_ = 0;
    dataIndex_ = tagsIndex_;
    bufSize_ = tagsIndex_;
    return true;
  }

  tagsIndex_ = index;
  index = parseString(buf, index, len);
  if (index < 0) {
    return false;
  }
  tagsLen_ = index - 1 - tagsIndex_;
  if (tagsLen_ == 1) {
    tagsLen_ = 0;
    dataIndex_ = tagsIndex_;
    bufSize_ = tagsIndex_;
    return true;
  }
  index = align(index);

  // Args
  if (!ensureArgIndexesCapacity(tagsLen_ - 1)) {
    return false;
  }
  dataIndex_ = index;
  index = parseArgs(buf, index, len);
  if (index < 0) {
    return false;
  }
  bufSize_ = index;

  return true;
}

bool OSCParser::fullMatch(int offset, const char *pattern) const {
  if (offset < 0 || addressLen_ < offset) {
    return false;
  }
  if (offset == addressLen_) {
    return strlen(pattern) == 0;
  }
  return strcmp(reinterpret_cast<char*>(&buf_[offset]), pattern) == 0;
}

int OSCParser::match(int offset, const char *pattern) const {
  if (offset < 0 || addressLen_ < offset) {
    return -1;
  }
  if (offset == addressLen_) {
    return offset;
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

bool OSCParser::strcmploc(const char *s1, const char *s2, int *loc) {
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

const char *OSCParser::getAddress() const {
  return reinterpret_cast<char*>(&buf_[0]);
}

int32_t OSCParser::getInt(int index) const {
  if (!isInt(index)) {
    return 0;
  }
  return getInt(&buf_[argIndexes_[index]]);
}

float OSCParser::getFloat(int index) const {
  if (!isFloat(index)) {
    return 0.0f;
  }
  int32_t i = getInt(&buf_[argIndexes_[index]]);
  float f;
  memcpy(&f, &i, 4);
  return f;
}

const char *OSCParser::getString(int index) const {
  if (!isString(index)) {
    return nullptr;
  }
  return reinterpret_cast<char *>(&buf_[argIndexes_[index]]);
}

int OSCParser::getBlobLength(int index) const {
  if (!isBlob(index)) {
    return 0;
  }
  int32_t size = getInt(&buf_[argIndexes_[index]]);
  if (size < 0) {
    size = 0;
  }
  return size;
}

const uint8_t *OSCParser::getBlob(int index) const {
  if (!isBlob(index)) {
    return nullptr;
  }
  return &buf_[argIndexes_[index] + 4];
}

int64_t OSCParser::getLong(int index) const {
  if (!isLong(index)) {
    return 0;
  }
  return getLong(&buf_[argIndexes_[index]]);
}

uint64_t OSCParser::getTime(int index) const {
  if (!isTime(index)) {
    return 0;
  }
  return getLong(&buf_[argIndexes_[index]]);
}

double OSCParser::getDouble(int index) const {
  if (!isDouble(index)) {
    return 0.0;
  }
  int64_t i = getLong(&buf_[argIndexes_[index]]);
  double d;
  memcpy(&d, &i, 8);
  return d;
}

int32_t OSCParser::getChar(int index) const {
  if (!isChar(index)) {
    return 0;
  }
  return getInt(&buf_[argIndexes_[index]]);
}

bool OSCParser::getBoolean(int index) const {
  if (!isBoolean(index)) {
    return false;
  }
  return buf_[tagsIndex_ + 1 + index] == 'T';
}

// --------------------------------------------------------------------------
//  Private functions
// --------------------------------------------------------------------------

bool OSCParser::ensureCapacity(int size) {
  // // Ensure there's a multiple of 4 bytes
  // size = ((size + 3)>>2)<<2;

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

bool OSCParser::ensureArgIndexesCapacity(int size) {
  if (size <= argIndexesCapacity_) {
    return true;
  }
  if (!dynamicArgIndexes_) {
    memoryErr_ = true;
    return false;
  }
  argIndexes_ = reinterpret_cast<int*>(
      realloc(argIndexes_, size * sizeof(int)));
  if (argIndexes_ == nullptr) {
    memoryErr_ = true;
    return false;
  }
  argIndexesCapacity_ = size;
  return true;
}

int OSCParser::parseString(const uint8_t *buf, int off, int len) {
  while (buf[off++] != 0) {
    if (off >= len) {
      return -1;
    }
  }
  return off;
}

int OSCParser::parseArgs(const uint8_t *buf, int off, int len) {
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
        int32_t size = getInt(&buf[off]);
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
