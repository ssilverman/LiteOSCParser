// OSCParser.cpp is part of OSCParser.
// (c) 2018 Shawn Silverman

#include "OSCParser.h"

// C++ includes
#include <cstdlib>
#include <cstring>

// Other includes
#include <Arduino.h>

OSCParser::OSCParser(int bufSize)
    : buf_(nullptr),
      bufSize_(0),
      bufCapacity_(0),
      dynamicBuf_(true),
      memoryErr_(false),
      addressLen_(0),
      tagsLen_(0),
      argIndexes_(nullptr),
      argIndexesCapacity_(0) {
  if (bufSize > 0) {
    dynamicBuf_ = false;
    bufCapacity_ = bufSize;
    buf_ = reinterpret_cast<uint8_t*>(malloc(bufSize));
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
  if (argIndexesCapacity_ < newArgCount) {
    argIndexes_ = reinterpret_cast<int *>(
        realloc(argIndexes_, newArgCount * sizeof(int)));
    if (argIndexes_ == nullptr) {
      memoryErr_ = true;
      return false;
    }
    argIndexesCapacity_ = newArgCount;
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
  bufSize_ = len;

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
  if (argIndexesCapacity_ < tagsLen_ - 1) {
    argIndexes_ = reinterpret_cast<int*>(realloc(argIndexes_,
                                                 (tagsLen_ - 1)*sizeof(int)));
    if (argIndexes_ == nullptr) {
      memoryErr_ = true;
      return false;
    }
    argIndexesCapacity_ = tagsLen_ - 1;
  }
  dataIndex_ = index;
  index = parseArgs(buf, index, len);
  if (index < 0) {
    return false;
  }

  return true;
}

bool OSCParser::fullMatch(int offset, const char *pattern) const {
  if (offset < 0 || addressLen_ < offset) {
    return false;
  }
  if (offset == addressLen_) {
    return strlen(pattern) == 0;
  }
  return strcmp(reinterpret_cast<char *>(&buf_[offset]), pattern) == 0;
}

int OSCParser::match(int offset, const char *pattern) const {
  if (offset < 0 || addressLen_ < offset) {
    return -1;
  }
  if (offset == addressLen_) {
    return offset;
  }

  int loc;
  if (strcmploc(reinterpret_cast<char *>(&buf_[offset]), pattern, &loc)) {
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

void OSCParser::getAddress(char *buf) const {
  memcpy(buf, &buf_[0], addressLen_);
  buf[addressLen_] = '\0';
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

int64_t OSCParser::getLong(int index) const {
  if (!isLong(index)) {
    return 0;
  }
  return getLong(&buf_[argIndexes_[index]]);
}

int64_t OSCParser::getTime(int index) const {
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

int OSCParser::getStringLength(int index) const {
  if (!isString(index)) {
    return 0;
  }
  return strlen(reinterpret_cast<char*>(&buf_[argIndexes_[index]]));
}

void OSCParser::getString(int index, char *buf) const {
  int slen = getStringLength(index);
  if (slen == 0) {
    return;
  }
  memcpy(buf, &buf_[argIndexes_[index]], slen);
  buf[slen] = '\0';
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

void OSCParser::getBlob(int index, uint8_t *buf) const {
  int blen = getBlobLength(index);
  if (blen == 0) {
    return;
  }
  memcpy(buf, &buf_[argIndexes_[index] + 4], blen);
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
  buf_ = reinterpret_cast<uint8_t *>(realloc(buf_, size));
  if (buf_ == nullptr) {
    memoryErr_ = true;
    return false;
  }
  bufCapacity_ = size;
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
