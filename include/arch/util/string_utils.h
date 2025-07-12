#pragma once

#include <string>

namespace Arch::Utils {

inline std::u32string utf8_to_utf32(const std::string_view &source_utf8) {
  std::u32string result;
  size_t i = 0;
  while (i < source_utf8.size()) {
    const auto byte = static_cast<unsigned char>(source_utf8[i]);
    char32_t codepoint = 0;
    size_t extra = 0;

    if (byte <= 0x7F) {
      codepoint = byte;
      extra = 0;
    } else if ((byte & 0xE0) == 0xC0) {
      codepoint = byte & 0x1F;
      extra = 1;
      if (codepoint < 0x2) goto invalid; // overlong
    } else if ((byte & 0xF0) == 0xE0) {
      codepoint = byte & 0x0F;
      extra = 2;
    } else if ((byte & 0xF8) == 0xF0) {
      codepoint = byte & 0x07;
      extra = 3;
      if (codepoint > 0x10) goto invalid;
    } else {
      goto invalid;
    }

    if (i + extra >= source_utf8.size()) goto invalid;

    for (size_t j = 1; j <= extra; j++) {
      const auto ch = static_cast<unsigned char>(source_utf8[i + j]);
      if ((ch & 0xC0) != 0x80) goto invalid;
      codepoint = (codepoint << 6) | (ch & 0x3F);
    }

    if ((codepoint >= 0xD800 && codepoint <= 0xDFFF) || codepoint > 0x10FFFF)
      goto invalid;

    result.push_back(codepoint);
    i += extra + 1;
    continue;

    invalid:
      result.push_back(U'?');
    i++;
  }
  return result;
}

inline std::string utf32_to_utf8(const std::u32string_view &source_utf32) {
  std::string result;
  for (const char32_t ch : source_utf32) {
    if (ch <= 0x7F) {
      result += static_cast<char>(ch);
    } else if (ch <= 0x7FF) {
      result += static_cast<char>(0xC0 | ((ch >> 6) & 0x1F));
      result += static_cast<char>(0x80 | (ch & 0x3F));
    } else if (ch <= 0xFFFF) {
      result += static_cast<char>(0xE0 | ((ch >> 12) & 0x0F));
      result += static_cast<char>(0x80 | ((ch >> 6) & 0x3F));
      result += static_cast<char>(0x80 | (ch & 0x3F));
    } else if (ch <= 0x10FFFF) {
      result += static_cast<char>(0xF0 | ((ch >> 18) & 0x07));
      result += static_cast<char>(0x80 | ((ch >> 12) & 0x3F));
      result += static_cast<char>(0x80 | ((ch >> 6) & 0x3F));
      result += static_cast<char>(0x80 | (ch & 0x3F));
    } else {
      /* Invalid Unicode code point, skip or use replacement char */
      result += '?';
    }
  }
  return result;
}

inline double u32_to_double(const std::u32string &source_utf32) {
  double result = 0, frac_base = 1;
  bool negative = false;
  int i = 0;
  if (source_utf32[0] == U'-') { i++; negative = true; }

  for ( ; i < source_utf32.size(); i++) {
    if (source_utf32[i] == U'.') { i++; break; }
    result = result * 10 + (source_utf32[i] - '0');
  }

  for (; i < source_utf32.size(); i++) {
    frac_base *= 0.1;
    result += (source_utf32[i] - '0') * frac_base;
  }

  if (negative) result = -result;
  return result;
}

}
