#pragma once

#include <iostream>

// ANSI style wrappers for color and effects
#define ANSI_RESET "\033[0m"
#define ANSI_GREEN(_str) (std::string("\033[32m") + (_str) + ANSI_RESET)
#define ANSI_BLUE(_str) (std::string("\033[94m") + (_str) + ANSI_RESET)
#define ANSI_YELLOW(_str) (std::string("\033[33m") + (_str) + "\033[0m")
#define ANSI_DIM(_str) (std::string("\033[2m") + (_str) + ANSI_RESET)
#define ANSI_CANCELLED(_str) (std::string("\033[9m\033[2m") + (_str) + ANSI_RESET)
#define ANSI_STRIKETHROUGH(_str) (std::string("\033[9m") + (_str) + ANSI_RESET)
#define ANSI_RED(_str) (std::string("\033[31m") + (_str) + ANSI_RESET)

// UTF-8 symbols for prompt UI
#define UTF_DIAMOND_EMPTY u8"\u25C7"
#define UTF_DIAMOND_FILLED u8"\u25C6"
#define UTF_BOX_EMPTY u8"\u25FB"
#define UTF_BLOCK_FILLED u8"\u25FC"
#define UTF_RADIO_EMPTY u8"\u25CB"
#define UTF_RADIO_FILLED u8"\u25CF"
#define UTF_CORNER_TOP_LEFT u8"\u250C"
#define UTF_VERTICAL_LINE u8"\u2502"
#define UTF_CORNER_BOTTOM_LEFT u8"\u2514"
#define UTF_TRIANGLE_UP u8"\u25B2"

#ifdef _WIN32
#include <windows.h>
#include <conio.h>

using TermCoord = COORD;

inline void setCursorVisible(bool visible) {
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_CURSOR_INFO info;
  GetConsoleCursorInfo(hConsole, &info);
  info.bVisible = visible;
  SetConsoleCursorInfo(hConsole, &info);
}

inline void moveCursorTo(TermCoord pos) {
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

inline TermCoord getCursorPosition() {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  return csbi.dwCursorPosition;
}

inline int getch_raw() {
  return _getch();
}

#else  // POSIX
#include <string>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

struct TermCoord {
  int X;
  int Y;
};

inline void setCursorVisible(bool visible) {
  std::cout << (visible ? "\033[?25h" : "\033[?25l") << std::flush;
}

inline void moveCursorTo(TermCoord pos) {
  std::cout << "\033[" << (pos.Y + 1) << ";" << (pos.X + 1) << "H" << std::flush;
}

inline TermCoord getCursorPosition() {
  struct termios original, raw;
  tcgetattr(STDIN_FILENO, &original);
  raw = original;
  raw.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &raw);

  std::cout << "\033[6n" << std::flush;
  char ch;
  std::string response;

  while (read(STDIN_FILENO, &ch, 1) == 1) {
    response += ch;
    if (ch == 'R') break;
  }

  tcsetattr(STDIN_FILENO, TCSANOW, &original);

  int rows = 0, cols = 0;
  if (sscanf(response.c_str(), "\033[%d;%dR", &rows, &cols) == 2) {
    return TermCoord{cols - 1, rows - 1};
  }
  return TermCoord{0, 0};
}

inline int getch_raw() {
  struct termios oldt, newt;
  int ch;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return ch;
}
#endif

// === TermCoord helpers ===

inline TermCoord offsetCoord(TermCoord base, int dx, int dy) {
#ifdef _WIN32
  return TermCoord{ static_cast<SHORT>(base.X + dx), static_cast<SHORT>(base.Y + dy) };
#else
  return TermCoord{ base.X + dx, base.Y + dy };
#endif
}

inline TermCoord addX(TermCoord base, int dx) {
  return offsetCoord(base, dx, 0);
}

inline TermCoord addY(TermCoord base, int dy) {
  return offsetCoord(base, 0, dy);
}

// === 通用光标操作 ===
inline void clearLineAt(TermCoord pos) {
  moveCursorTo(pos);
  std::cout << "\033[K" << std::flush;
}

inline void clearBelowLine(TermCoord pos, int count) {
  for (int i = 0; i < count; ++i)
    clearLineAt(addY(pos, i));
}

inline void clearAboveLine(TermCoord pos, int count) {
  for (int i = 1; i <= count; ++i)
    clearLineAt(addY(pos, -i));
}

// === 键盘事件与解析 ===
enum class Key {
  Unknown = -1,
  Char,
  Enter,
  Backspace,
  ArrowUp,
  ArrowDown,
  ArrowLeft,
  ArrowRight,
  Escape,
  CtrlC,
};

struct KeyEvent {
  Key key;
  char ch; // 仅当 key == Char 时有效
};

inline KeyEvent get_key_event() {
  int ch1 = getch_raw();

#ifdef _WIN32
  if (ch1 == 3) return {Key::CtrlC, 0};
  if (ch1 == 0 || ch1 == 224) {
    int ch2 = getch_raw();
    switch (ch2) {
      case 72: return {Key::ArrowUp, 0};
      case 80: return {Key::ArrowDown, 0};
      case 75: return {Key::ArrowLeft, 0};
      case 77: return {Key::ArrowRight, 0};
      default: return {Key::Unknown, 0};
    }
  } else if (ch1 == 13) return {Key::Enter, 0};
  else if (ch1 == 8) return {Key::Backspace, 0};
  else if (ch1 == 27) return {Key::Escape, 0};
  else return {Key::Char, static_cast<char>(ch1)};
#else
  if (ch1 == 3) return {Key::CtrlC, 0};
  if (ch1 == 27) {
    char seq1 = getchar();
    if (seq1 == '[') {
      char seq2 = getchar();
      switch (seq2) {
        case 'A': return {Key::ArrowUp, 0};
        case 'B': return {Key::ArrowDown, 0};
        case 'C': return {Key::ArrowRight, 0};
        case 'D': return {Key::ArrowLeft, 0};
        default: return {Key::Unknown, 0};
      }
    }
    return {Key::Escape, 0};
  } else if (ch1 == 10 || ch1 == 13) return {Key::Enter, 0};
  else if (ch1 == 127) return {Key::Backspace, 0};
  else return {Key::Char, static_cast<char>(ch1)};
#endif
}
