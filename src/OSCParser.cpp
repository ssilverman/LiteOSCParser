// OSCParser.cpp is part of OSCParser.
// (c) 2018 Shawn Silverman

#include "OSCParser.h"

// C++ includes
#include <cstdlib>
#include <cstring>

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
  addressLen_ = 0;
  tagsLen_ = 0;

  if ((len & 0x03) != 0 || len <= 0) {
    return false;
  }

  int index = 0;

  // Address
  if (buf[index] != '/') {
    return false;
  }
  addressIndex_ = index;
  index = parseString(buf, index, len);
  if (index < 0) {
    return false;
  }
  addressLen_ = index - 1 - addressIndex_;
  index = alignOffset(index);

  // Copy everything into our local buffer
  if (bufSize_ < len) {
    buf_ = reinterpret_cast<uint8_t*>(realloc(buf_, len));
    if (buf_ == nullptr) {
      return false;
    }
  }
  memcpy(buf_, buf, len);

  // Type tags

  // No tags
  if (index >= len || buf[index] != ',') {
    return true;
  }

  tagsIndex_ = index + 1;
  index = parseString(buf, index, len);
  if (index < 0) {
    return false;
  }
  tagsLen_ = index - 1 - tagsIndex_;
  if (tagsLen_ == 0) {
    return true;
  }
  index = alignOffset(index);

  // Args
  if (argIndexesSize_ < tagsLen_) {
    argIndexes_ = reinterpret_cast<int*>(realloc(argIndexes_,
                                                 tagsLen_ * sizeof(int)));
    if (argIndexes_ == nullptr) {
      return false;
    }
  }
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
  return strcmp(reinterpret_cast<char *>(&buf_[addressIndex_ + offset]),
                pattern) == 0;
}

int OSCParser::match(int offset, const char *pattern) const {
  if (offset < 0 || addressLen_ < offset) {
    return -1;
  }
  if (offset == addressLen_) {
    return offset;
  }

  int loc;
  if (strcmploc(reinterpret_cast<char *>(&buf_[addressIndex_ + offset]),
                pattern, &loc)) {
    return addressLen_;
  }
  loc += offset;

  // There's a match if the next character is a '/'
  // Strictly speaking, we don't need to check if loc is in range
  // because one past the end is the NULL terminator, and that's
  // valid memory
  if (buf_[addressIndex_ + loc] == '/') {
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
  memcpy(buf, &buf_[addressIndex_], addressLen_);
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
  size_t slen = getStringLength(index);
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
  size_t blen = getBlobLength(index);
  if (blen == 0) {
    return;
  }
  memcpy(buf, &buf_[argIndexes_[index] + 4], blen);
}

bool OSCParser::getBoolean(int index) const {
  if (!isBoolean(index)) {
    return false;
  }
  return buf_[tagsIndex_ + index] == 'T';
}

// --------------------------------------------------------------------------
//  Private functions
// --------------------------------------------------------------------------

int OSCParser::parseString(const uint8_t *buf, int off, int len) {
  while (buf[off++] != 0) {
    if (off >= len) {
      return -1;
    }
  }
  return off;
}

int OSCParser::parseArgs(const uint8_t *buf, int off, int len) {
  for (int i = 0; i < tagsLen_; i++) {
    argIndexes_[i] = off;
    switch (buf[i + tagsIndex_]) {
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
        off = alignOffset(off);
        break;

      case 'b': {  // OSC-blob
        if (off + 4 > len) {
          return -1;
        }
        int32_t size = getInt(&buf[off]);
        if (size < 0) {
          return -1;
        }
        off = alignOffset(off + 4 + size);
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
