#include <ion/unicode/utf8_decoder.h>
#include <assert.h>

static inline int leading_ones(uint8_t value) {
  for (int i=0; i<8; i++) {
    if (!(value & 0x80)) {
      return i;
    }
    value = value << 1;
  }
  assert(false);
}

static inline uint8_t last_k_bits(uint8_t value, uint8_t bits) {
  return (value & ((1<<bits)-1));
}

CodePoint UTF8Decoder::nextCodePoint() {
  assert(m_stringPosition == m_stringPosition || *(m_stringPosition - 1) != 0);
  int leadingOnes = leading_ones(*m_stringPosition);
  uint32_t result = last_k_bits(*m_stringPosition++, 8-leadingOnes-1);
  for (int i = 0; i < leadingOnes - 1; i++) {
    result <<= 6;
    result += (*m_stringPosition++ & 0x3F);
  }
  return CodePoint(result);
}

CodePoint UTF8Decoder::previousCodePoint() {
  assert(m_stringPosition > m_string);
  if (leading_ones(*(m_stringPosition - 1)) == 0) {
    // The current code point is one char long
    m_stringPosition--;
    return *m_stringPosition;
  }
  // The current code point spans over multiple chars
  uint32_t result = 0;
  int i = 0;
  int leadingOnes = 1;
  m_stringPosition--;
  assert(leading_ones(*m_stringPosition) == 1);
  while (leadingOnes == 1) {
    assert(m_stringPosition > m_string);
    result += (*m_stringPosition & 0x3F) << (6 * i);
    i++;
    m_stringPosition--;
    leadingOnes = leading_ones(*m_stringPosition);
  }

  assert(i <= 3);
  assert(leadingOnes > 1 && leadingOnes <= 4);
  assert(m_stringPosition >= m_string);

  result+= last_k_bits(*m_stringPosition, 8-leadingOnes-1);
  return CodePoint(result);
}

size_t UTF8Decoder::CharSizeOfCodePoint(CodePoint c) {
  constexpr int bufferSize = CodePoint::MaxCodePointCharLength;
  char buffer[bufferSize];
  return CodePointToChars(c, buffer, bufferSize);
}

size_t UTF8Decoder::CodePointToChars(CodePoint c, char * buffer, int bufferSize) {
  if (bufferSize <= 0) {
    return 0;
  }
  size_t i = 0;
  if (c <= 0x7F) {
    buffer[i++] = c;
  } else if (c <= 0x7FF) {
    buffer[i++] = 0b11000000 | (c >> 6);
    if (bufferSize <= i) { return i; }
    buffer[i++] = 0b10000000 | (c & 0b111111);
  } else if (c <= 0xFFFF) {
    buffer[i++] = 0b11100000 | (c >> 12);
    if (bufferSize <= i) { return i; }
    buffer[i++] = 0b10000000 | ((c >> 6) & 0b111111);
    if (bufferSize <= i) { return i; }
    buffer[i++] = 0b10000000 | (c & 0b111111);
  } else {
    buffer[i++] = 0b11110000 | (c >> 18);
    if (bufferSize <= i) { return i; }
    buffer[i++] = 0b10000000 | ((c >> 12) & 0b111111);
    if (bufferSize <= i) { return i; }
    buffer[i++] = 0b10000000 | ((c >> 6) & 0b111111);
    if (bufferSize <= i) { return i; }
    buffer[i++] = 0b10000000 | (c & 0b111111);
  }
  return i;
}