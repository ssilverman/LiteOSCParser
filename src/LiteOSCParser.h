// LiteOSCParser.h defines an OSC processor.
// This is part of LiteOSCParser.
// (c) 2018 Shawn Silverman

#ifndef LITEOSCPARSER_H_
#define LITEOSCPARSER_H_

// C++ includes
#include <cstdint>

namespace qindesign {
namespace osc {

// LiteOSCParser parses and constructs OSC messages. The internal buffer
// and argument list can be either dynamically allocated or set to a
// specific size. Any functions that add to, initialize, or change the
// message return whether they were successful. One of the possible
// reasons for failure is that there's not enough space in one of the
// internal buffers, and more cannot be allocated. The isMemoryError()
// function can be used to determine this case.
//
// One strategy when creating a message would be to add everything needed
// and then check for a memory error afterwards, instead of after every
// addition.
//
// Some of the functions return a pointer to a spot in the internal buffer.
// Try not to mismanage those. :)
class LiteOSCParser {
 public:
  // Creates a new OSC parser. The maximum buffer size and argument count
  // are given. If the buffer size is non-positive then the buffer will be
  // dynamically allocated as needed. The size should be a multiple of four.
  //
  // Similarly, if the maximum argument count, maxArgCount, is non-positive,
  // then the internal array will be dynamically allocated as needed.
  // Otherwise, the maxmimum number of arguments in a message will be
  // limited to that count.
  //
  // The buffer size, bufSize, is given in bytes, and the maximum argument
  // count, maxArgCount, is given in ints, i.e. maxArgCount*sizeof(int).
  LiteOSCParser(int bufSize, int maxArgCount);

  // Initializes a new OSC parser having dynamic buffer and argument
  // allocation.
  LiteOSCParser() : LiteOSCParser(0, 0) {}

  // Not copyable
  LiteOSCParser(const LiteOSCParser &) = delete;
  LiteOSCParser &operator=(const LiteOSCParser &) = delete;

  ~LiteOSCParser();

  // ------------------------------------------------------------------------
  //  Creating
  // ------------------------------------------------------------------------

  // Initializes the message with a new address and no arguments. This
  // returns whether the initialization was successful. This will be
  // unsuccessful if the address does not start with a '/' or if there
  // isn't enough space in the internal buffer. The second condition can
  // be checked with a call to isMemoryError().
  bool init(const char *address);

  // Clears the message. Under the covers, this calls init with an
  // empty string and then ignores the result.
  void clear() {
    init("");
  }

  // Gets the total size of the encoded message.
  int getMessageSize() const {
    return bufSize_;
  }

  // Gets a pointer to the internal buffer, for sending the data elsewhere.
  // See getMessageSize() to retrieve the size.
  const uint8_t *getMessageBuf() const {
    return buf_;
  }

  // Adds a 32-bit int argument.
  bool addInt(int32_t i);

  // Adds a 32-bit float argument.
  bool addFloat(float f);

  // Adds a string.
  bool addString(const char *s);

  // Adds a blob having the given length.
  bool addBlob(const uint8_t *b, int len);

  // Adds a 64-bit long argument.
  bool addLong(int64_t h);

  // Adds a 64-bit time argument.
  bool addTime(uint64_t t);

  // Adds a 64-bit double argument.
  bool addDouble(double d);

  // Adds a boolean.
  bool addBoolean(bool b);

  // Returns whether an insufficient buffer size is preventing the latest
  // message from being constructed.
  bool isMemoryError() const {
    return memoryErr_;
  }

  // ------------------------------------------------------------------------
  //  Parsing and matching
  // ------------------------------------------------------------------------

  // Parses the given buffer and returns whether the message is valid.
  // If this returns 'false' then isMemoryError() can be used to determine
  // whether the failure was due to not enough space in the internal buffer.
  bool parse(const uint8_t *buf, int len);

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

  // ------------------------------------------------------------------------
  //  Getters
  // ------------------------------------------------------------------------

  // Gets a pointer to the address. This returns a pointer into the
  // internal buffer.
  const char *getAddress() const;

  // Returns the current argument count.
  int getArgCount() const {
    if (tagsLen_ == 0) {
      return 0;
    }
    return tagsLen_ - 1;
  }

  // Returns the tag for the argument at the given index. This returns
  // '\0' if the index is out of range.
  char getTag(int index) const {
    if (index < 0 || tagsLen_ - 1 <= index) {
      return '\0';
    }
    return buf_[tagsIndex_ + index + 1];
  }

  // Checks if the argument at the given index is a 32-bit int.
  bool isInt(int index) const {
    return isTag(index, 'i');
  }

  // Checks if the argument at the given index is a 32-bit float.
  bool isFloat(int index) const {
    return isTag(index, 'f');
  }

  // Checks if the argument at the given index is a string.
  bool isString(int index) const {
    return isTag(index, 's');
  }

  // Checks if the argument at the given index is a blob.
  bool isBlob(int index) const {
    return isTag(index, 'b');
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

  // Checks if the argument at the given index is a char.
  bool isChar(int index) const {
    return isTag(index, 'c');
  }

  // Checks if the argument at the given index is a boolean.
  bool isBoolean(int index) const {
    return isTag(index, 'T') || isTag(index, 'F');
  }

  // Checks if the argument at the given index is a null.
  bool isNull(int index) const {
    return isTag(index, 'N');
  }

  // Checks if the argument at the given index is an impulse.
  bool isImpulse(int index) const {
    return isTag(index, 'I');
  }

  // Gets the 32-bit int at the given index. This will return zero
  // if the index is out of range or if the argument is the wrong type.
  int32_t getInt(int index) const;

  // Gets the 32-bit float at the given index. This will return zero
  // if the index is out of range or if the argument is the wrong type.
  float getFloat(int index) const;

  // Gets a pointer to the string stored at the given index. This returns
  // a pointer into the internal buffer, or nullptr if the index if out
  // of range or if the argument is the wrong type.
  const char *getString(int index) const;

  // Gets the blob length at the given index. This will return zero
  // if the index is out of range or if the argument is the wrong type.
  int getBlobLength(int index) const;

  // Gets a pointer to the stored stored at the given index. This returns
  // a pointer into the internal buffer, or nullptr if the index if out
  // of range or if the argument is the wrong type. See getBlobLength
  // to get the blob size.
  const uint8_t *getBlob(int index) const;

  // Gets the 64-bit long at the given index. This will return zero
  // if the index is out of range or if the argument is the wrong type.
  int64_t getLong(int index) const;

  // Gets the 64-bit OSC-timetag at the given index. This will return zero
  // if the index is out of range or if the argument is the wrong type.
  //
  // The format is the representation used by NTP time stamps, where the
  // top 32 bits are the number of seconds and the bottom 32 bits are the
  // fractional part.
  // See RFC 1305 (NTP) and RFC 2030 (SNTP).
  uint64_t getTime(int index) const;

  // Gets the 64-bit double at the given index. This will return zero
  // if the index is out of range or if the argument is the wrong type.
  double getDouble(int index) const;

  // Gets the 32-bit char at the given index. This will return zero
  // if the index is out of range or if the argument is the wrong type.
  int32_t getChar(int index) const;

  // Gets the boolean value at the given index. This will return false
  // if the index is out of range or if the argument is the wrong type.
  bool getBoolean(int index) const;

 private:
  // Ensures that we have enough buffer capacity. This returns whether
  // we do, allocating if necessary. If there isn't enough space then
  // the memory error condition will be set to 'true'.
  //
  // This does not ensure size is the next multiple of 4.
  bool ensureCapacity(int size);

  // Ensures that we have enough capacity for the argument indexes. This
  // returns whether we do, allocating if necessary. If there isn't enough
  // space then the memory error condition will be set to 'true'.
  bool ensureArgIndexesCapacity(int size);

  // Aligns the given number to a multiple of 4.
  static int align(int n) {
    return ((n + 3) >> 2) << 2;
    // return off + ((4 - (off & 0x03)) & 0x03);
  }

  // Adds one argument having the specified size to the buffer. Things
  // are shifted around appropriately. This returns whether the addition
  // was a success. As well, bufSize_ will be set to the correct total size.
  //
  // argSize is not expected to be a multiple of 4, but it is sized
  // internally so that it is. Additionally, any extra allocated bytes
  // are set to zero.
  bool addArg(char tag, int argSize);

  // Parses a string starting at 'off' and returns the index just past
  // the NULL terminator. This will return a negative value if the end
  // of the string could not be found.
  static int parseString(const uint8_t *buf, int off, int len);

  // Parses all the arguments starting at 'off' and returns the index
  // just past the NULL terminator. This will return a negative value
  // if an error is encountered.
  int parseArgs(const uint8_t *buf, int off, int len);

  // Checks if the index is in range and the tag matches.
  bool isTag(int index, char tag) const {
    return (0 <= index && index < tagsLen_ - 1) &&
           buf_[tagsIndex_ + index + 1] == tag;
  }

  // Gets a big-endian-encoded uint32 from the given buffer.
  static uint32_t getUint(const uint8_t *buf) {
    return uint32_t{buf[0]} << 24 |
           uint32_t{buf[1]} << 16 |
           uint32_t{buf[2]} << 8 |
           uint32_t{buf[3]};
  }

  // Stores a big-endian-encoded uint32 into the given buffer.
  static void setUint(uint8_t *buf, uint32_t i) {
    buf[0] = i >> 24;
    buf[1] = i >> 16;
    buf[2] = i >> 8;
    buf[3] = i;
  }

  // Gets a big-endian-encoded uint64 from the given buffer.
  static uint64_t getUlong(const uint8_t *buf) {
    return (uint64_t{getUint(buf)} << 32) | uint64_t{getUint(buf + 4)};
  }

  // Stores a big-endian-encoded uint64 into the given buffer.
  static void setUlong(uint8_t *buf, uint64_t h) {
    setUint(buf, h >> 32);
    setUint(buf + 4, h);
  }

  // Similar to strcmp, this returns whether the two strings match, but
  // if they do not match, then loc is set to the location of the first
  // mismatch.
  // Named in honour of the naming of yore.
  static bool strcmploc(const char *s1, const char *s2, int *loc);

  // Buffer for holding the message
  uint8_t *buf_;
  int bufSize_;  // Invariant: bufSize_ % 4 == 0
  int bufCapacity_;
  bool dynamicBuf_;
  bool memoryErr_;

  // All the parts
  int addressLen_;
  int tagsIndex_;  // Invariant: tagsIndex_ % 4 == 0
  int tagsLen_;  // Includes the ',' if there are tags, zero otherwise
  int dataIndex_;  // Invariant: dataIndex_ % 4 == 0

  // Args
  int *argIndexes_;
  int argIndexesCapacity_;
  bool dynamicArgIndexes_;
};

// OSCBundle is a container for OSC messages and other OSC bundles.
// The internal buffer can be either dynamically allocated or set to a
// specific size. Any functions that add to, initialize, or change the
// bundle return whether they were successful. One of the possible
// reasons for failure is that there's not enough space in one of the
// internal buffers, and more cannot be allocated. The isMemoryError()
// function can be used to determine this case.
class OSCBundle {
 public:
  // Creates a new OSCBundle. The bundle time and maximum buffer size
  // are given. If the maximum buffer size is positive and less than 16,
  // then it will be set to 16. If it is non-positive, however, then the
  // buffer will be dynamically allocated as needed. It should be a
  // multiple of four.
  OSCBundle(int bufCapacity);

  // Creates a new OSCBundle that has a dynamic buffer.
  OSCBundle() : OSCBundle(0) {}

  // Not copyable
  OSCBundle(const OSCBundle &) = delete;
  OSCBundle &operator=(const OSCBundle &) = delete;

  ~OSCBundle();

  // Resets the contents and uses the given time for the bundle.
  // This also resets the memory error condition.
  //
  // This must be called at least once before messages or bundles can
  // be added.
  bool init(uint64_t time);

  // Returns the total size of the bundle.
  int size() const {
    return bufSize_;
  }

  // Gets a pointer to the internal buffer, for sending the data elsewhere.
  // See size() to retrieve the size. Play nice with this pointer.
  const uint8_t *buf() const {
    return buf_;
  }

  // Returns whether an insufficient buffer size is preventing the latest
  // content from being added.
  bool isMemoryError() const {
    return memoryErr_;
  }

  // Adds an OSC message to this bundle. This will return false if
  // the message could not be added, either due to init not being
  // called at least once or insufficient memory.
  bool addMessage(const LiteOSCParser &osc);

  // Adds another OSC bundle to this bundle. This will return false if
  // the message could not be added, either due to init not being called
  // at least once or insufficient memory.
  bool addBundle(const OSCBundle &bundle);

  // Parses the given buffer and returns whether it is a valid bundle. This
  // recursively looks into sub-bundles. If a bundle element is an OSC message
  // then a rudimentary check for starting with a '/' character is performed.
  static bool parse(const uint8_t *buf, int32_t len);

 private:
  // Adds content to the bundle. This returns false if init has not
  // been called at least once.
  bool add(const uint8_t *buf, int size);

  // Ensures that we have enough buffer capacity. This returns whether
  // we do, allocating if necessary. If there isn't enough space then
  // the memory error condition will be set to 'true'.
  bool ensureCapacity(int size);

  uint8_t *buf_;
  int bufSize_;
  int bufCapacity_;
  bool dynamicBuf_;
  bool memoryErr_;

  bool isInitted_;
};

}  // namespace osc
}  // namespace qindesign

#endif  // LITEOSCPARSER_H_
