#pragma once

namespace Arch {

inline bool is_Alpha(const char32_t c) { return (c >= U'A' && c <= U'Z') || (c >= U'a' && c <= U'z'); }
inline bool is_ALPHA(const char32_t c) { return (c >= U'A' && c <= U'Z'); }
inline bool is_lower(const char32_t c) { return (c >= U'a' && c <= U'z'); }

inline bool is_Greek(const char32_t c) { return (c >= 0x0391 && c <= 0x03A9) || (c >= 0x03B1 && c <= 0x03C9); }
inline bool is_GREEK(const char32_t c) { return c >= 0x0391 && c <= 0x03A9; }
inline bool is_greek(const char32_t c) { return c >= 0x03B1 && c <= 0x03C9; }

inline bool is_Number(const char32_t c) { return (c >= U'0' && c <= U'9'); }

}
