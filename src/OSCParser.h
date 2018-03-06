// OSCParser.h defines an OSC processor.
// This is part of the OSCParser library.
// (c) 2018 Shawn Silverman

#ifndef OSCPARSER_H_
#define OSCPARSER_H_

// C++ includes
#include <cstddef>
#include <cstdint>

class OSCParser {
 public:
  OSCParser()
      : buf_(nullptr),
        bufSize_(0),
        argIndexes_(nullptr),
        argIndexesSize_(0) {}
  ~OSCParser() = default;

  // Parses the given buffer.
  bool parse(const uint8_t *buf, int len);

  // Gets the address length, not including the NULL terminator.
  size_t getAddressLength() const {
    return static_cast<size_t>(addressLen_);
  }

  // Copies the address into the given buffer. The buffer must be at least
  // large enough to hold the address plus the NULL terminator.
  void getAddress(char *buf) const;

  // Returns whether the address fully matches the given pattern,
  // starting at offset in the address.
  bool fullMatch(int offset, const char *pattern) const;

  // Checks whether the address partially matches the given pattern and
  // returns an index one past the last character matched. This starts
  // matching at 'offset' in the address. This does not match partial
  // containers; for a match to happen, the next character of the address
  // must be a '/'.
  //
  // This will return -1 if offset is out of range and zero if there is
  // no match.
  int match(int offset, const char *pattern) const;

  // Returns the current argument count.
  int argCount() const {
    return tagsLen_;
  }

  // Checks if the argument at the given index is a 32-bit int.
  bool isInt(int index) const {
    return isTag(index, 'i');
  }

  // Checks if the argument at the given index is a 32-bit float.
  bool isFloat(int index) const {
    return isTag(index, 'f');
  }

  // Checks if the argument at the given index is a 64-bit long.
  bool isLong(int index) const {
    return isTag(index, 'h');
  }

  // Checks if the argument at the given index is an OSC-timetag.
  bool isTime(int index) const {
    return isTag(index, 't');
  }

  // Checks if the argument at the given index is a 64-bit double.
  bool isDouble(int index) const {
    return isTag(index, 'd');
  }

  // Checks if the argument at the given index is a string.
  bool isString(int index) const {
    return isTag(index, 's');
  }

  // Checks if the argument at the given index is a blob.
  bool isBlob(int index) const {
    return isTag(index, 'b');
  }

  // Checks if the argument at the given index is a boolean.
  bool isBoolean(int index) const {
    return isTag(index, 'T') || isTag(index, 'F');
  }

  // Gets the 32-bit int at the given index. This will return zero
  // if the index is out of range or if the argument is the wrong type.
  int32_t getInt(int index) const;

  // Gets the 32-bit float at the given index. This will return zero
  // if the index is out of range or if the argument is the wrong type.
  float getFloat(int index) const;

  // Gets the 64-bit long at the given index. This will return zero
  // if the index is out of range or if the argument is the wrong type.
  int64_t getLong(int index) const;

  // Gets the 64-bit OSC-timetag at the given index. This will return zero
  // if the index is out of range or if the argument is the wrong type.
  int64_t getTime(int index) const;

  // Gets the 64-bit double at the given index. This will return zero
  // if the index is out of range or if the argument is the wrong type.
  double getDouble(int index) const;

  // Gets the string length at the given index. This will return zero
  // if the index is out of range or if the argument is the wrong type.
  size_t getStringLength(int index) const;

  // Gets the string at the given index and stores if in the given buffer.
  // A NULL terminator will be added always. This will return the string
  // length actually copied into the buffer, not including the NULL
  // terminator.
  void getString(int index, char *buf) const;

  // Gets the blob length at the given index. This will return zero
  // if the index is out of range or if the argument is the wrong type.
  size_t getBlobLength(int index) const;

  // Gets the blob at the given index and stores if in the given buffer.
  // This will return the blob length actually copied into the buffer.
  void getBlob(int index, uint8_t *buf) const;

  // Gets the boolean value at the given index. This will return false
  // if the index is out of range or if the argument is the wrong type.
  bool getBoolean(int index) const;

 private:
  // Parses a string starting at 'off' and returns the index just past
  // the NULL terminator. This will return a negative value if the end
  // of the string could not be found.
  static int parseString(const uint8_t *buf, int off, int len);

  // Parses all the arguments starting at 'off' and returns the index
  // just past the NULL terminator. This will return a negative value
  // if an error is encountered.
  int parseArgs(const uint8_t *buf, int off, int len);

  // Returns an aligned offset, given the current offset.
  static int alignOffset(int off) {
    return off + ((4 - (off & 0x03)) & 0x03);
  }

  // Checks if the index is in range and the tag matches.
  bool isTag(int index, char tag) const {
    return (0 <= index && index < tagsLen_) &&
           buf_[tagsIndex_ + index] == tag;
  }

  // Gets a big-endian-encoded int32 from the given buffer.
  static int32_t getInt(const uint8_t *buf) {
    return int32_t{*(buf+0)} << 24 |
           int32_t{*(buf+1)} << 16 |
           int32_t{*(buf+2)} << 8 |
           int32_t{*(buf+3)};
  }

  // Gets a big-endian-encoded int64 from the given buffer.
  static int64_t getLong(const uint8_t *buf) {
    return (int64_t{getInt(buf)} << 32) | int64_t{getInt(buf + 4)};
  }

  // Similar to strcmp, this returns whether the two strings match, but
  // if they do not match, then loc is set to the location of the first
  // mismatch.
  // Named in honour of the naming of yore.
  static bool strcmploc(const char *s1, const char *s2, int *loc);

  // Buffer for holding the message
  uint8_t *buf_;
  int bufSize_;

  // All the parts
  int addressIndex_;
  int addressLen_;
  int tagsIndex_;
  int tagsLen_;

  // Args
  int *argIndexes_;
  int argIndexesSize_;
};

#endif  // OSCPARSER_H_
