#include <conio.h>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <windows.h>

// ANSI style wrappers for color and effects
#define ANSI_RESET "\033[0m"
#define ANSI_GREEN(_str) (std::string("\033[32m") + (_str) + ANSI_RESET)
#define ANSI_BLUE(_str) (std::string("\033[94m") + (_str) + ANSI_RESET)
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

enum PromptState { Activated, Successed, Failed, Invisible };
enum BooleanChoice { Yes, No };

#define ICON_PROMPT(_state)                                                    \
  ((_state) == PromptState::Activated   ? ANSI_GREEN(UTF_DIAMOND_FILLED)       \
   : (_state) == PromptState::Successed ? ANSI_GREEN(UTF_DIAMOND_EMPTY)        \
                                        : ANSI_RED(UTF_BLOCK_FILLED))

inline std::string ICON_BOOLEAN(bool selected, const std::string &label) {
  return selected ? (ANSI_GREEN(std::string(UTF_RADIO_FILLED)) + " " + label)
                  : ANSI_DIM(std::string(UTF_RADIO_EMPTY) + " " + label);
}

inline std::string ICON_RADIO(bool selected) {
  return selected ? ANSI_GREEN(UTF_BLOCK_FILLED) : ANSI_GREEN(UTF_BOX_EMPTY);
}

void setCursorVisible(bool visible) {
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_CURSOR_INFO info;
  GetConsoleCursorInfo(hConsole, &info);
  info.bVisible = visible;
  SetConsoleCursorInfo(hConsole, &info);
}

/* Abstract Prompt */
struct CLI_PROMPT {
  PromptState state = PromptState::Activated;
  virtual void prompt(COORD pos) const = 0;
  virtual bool run(bool isLastPrompt) = 0;
  virtual ~CLI_PROMPT() = default;
};

/* Yes/No Continue Prompt */
struct CLI_PromptContinue : CLI_PROMPT {
  std::string label;
  BooleanChoice choice = Yes;

  CLI_PromptContinue(std::string text) : label(std::move(text)) {}

  void prompt(COORD pos) const override {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

    std::cout << ANSI_BLUE(UTF_VERTICAL_LINE) << "  ";
    std::cout << ICON_BOOLEAN(choice == Yes, "Yes") << " / "
              << ICON_BOOLEAN(choice == No, "No");
    std::cout << "\n" << ANSI_BLUE(UTF_CORNER_BOTTOM_LEFT);

    std::cout << "\033[K";
    std::cout.flush();
  }

  bool run(bool isLastPrompt) override {
    std::cout << ICON_PROMPT(state) << "  " << label << "\n";
    std::cout << UTF_VERTICAL_LINE << "\n";

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    COORD pos = csbi.dwCursorPosition;
    pos.Y -= 1;

    while (true) {
      prompt(pos);
      int c = _getch();
      if (c == 0 || c == 224) {
        int key = _getch();
        if (key == 75)
          choice = Yes;
        if (key == 77)
          choice = No;
      } else if (c == 13) {
        state = choice == Yes ? PromptState::Successed : PromptState::Failed;

        // 清除选择和底线行
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
        std::cout << "\033[K";
        COORD below = pos;
        below.Y += 1;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), below);
        std::cout << "\033[K";

        // 重绘首行图标
        COORD top = pos;
        top.Y -= 1;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), top);
        std::cout << ICON_PROMPT(state) << "  " << label << "\033[K";

        // 重绘 prompt 内容（Yes/No 结果）
        // prompt(pos);
        if (choice == Yes)
          std::cout << "\n"
                    << UTF_VERTICAL_LINE << "  " << ANSI_DIM("Yes") << "\n"
                    << UTF_VERTICAL_LINE << "\n"
                    << UTF_VERTICAL_LINE << "\n";
        else {
          std::cout << "\n"
                    << UTF_VERTICAL_LINE << "  " << ANSI_DIM("No") << "\n"
                    << UTF_VERTICAL_LINE << "\n"
                    << UTF_CORNER_BOTTOM_LEFT << ANSI_RED("  Exiting.")
                    << "\n\n";
          exit(0);
        }
        return choice == Yes;
      } else if (c == 3) { // Ctrl-C 被按下
      cancelled:
        state = PromptState::Failed;

        // 清除选择和底线行
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
        std::cout << "\033[K";
        COORD below = pos;
        below.Y += 1;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), below);
        std::cout << "\033[K";

        // 重绘首行图标为失败
        COORD top = pos;
        top.Y -= 1;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), top);
        std::cout << ICON_PROMPT(state) << "  " << label << "\033[K";
        // 重绘 prompt 内容（Yes/No 结果）
        std::cout << "\n"
                  << UTF_VERTICAL_LINE << "  " << ANSI_CANCELLED((choice == Yes ? "Yes" : "No")) << "\n"
                  << UTF_VERTICAL_LINE << "\n"
                  << UTF_CORNER_BOTTOM_LEFT << ANSI_RED("  Exiting.") << "\n\n";
        exit(1); // 立即退出
      }
    }
  }
};

/* Yes/No Continue Prompt */
struct CLI_PromptBoolean : CLI_PROMPT {
  std::string label;
  BooleanChoice choice = Yes;

  CLI_PromptBoolean(std::string text) : label(std::move(text)) {}

  void prompt(COORD pos) const override {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

    std::cout << ANSI_BLUE(UTF_VERTICAL_LINE) << "  ";
    std::cout << ICON_BOOLEAN(choice == Yes, "Yes") << " / "
              << ICON_BOOLEAN(choice == No, "No");
    std::cout << "\n" << ANSI_BLUE(UTF_CORNER_BOTTOM_LEFT);

    std::cout << "\033[K";
    std::cout.flush();
  }

  bool run(bool isLastPrompt) override {
    std::cout << ICON_PROMPT(state) << "  " << label << "\n";
    std::cout << UTF_VERTICAL_LINE << "\n";

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    COORD pos = csbi.dwCursorPosition;
    pos.Y -= 1;

    while (true) {
      prompt(pos);
      int c = _getch();
      if (c == 0 || c == 224) {
        int key = _getch();
        if (key == 75)
          choice = Yes;
        if (key == 77)
          choice = No;
      } else if (c == 13) {
        state = PromptState::Successed;

        // 清除选择和底线行
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
        std::cout << "\033[K";
        COORD below = pos;
        below.Y += 1;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), below);
        std::cout << "\033[K";

        // 重绘首行图标
        COORD top = pos;
        top.Y -= 1;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), top);
        std::cout << ICON_PROMPT(state) << "  " << label << "\033[K";

        // 重绘 prompt 内容（Yes/No 结果）
          std::cout << "\n"
                    << UTF_VERTICAL_LINE << "  " << ANSI_DIM((choice == Yes ? "Yes" : "No")) << "\n"
                    << UTF_VERTICAL_LINE << "\n"
                    << UTF_VERTICAL_LINE << "\n";
        return true;
      } else if (c == 3) { // Ctrl-C 被按下
      cancelled:
        state = PromptState::Failed;

        // 清除选择和底线行
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
        std::cout << "\033[K";
        COORD below = pos;
        below.Y += 1;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), below);
        std::cout << "\033[K";

        // 重绘首行图标为失败
        COORD top = pos;
        top.Y -= 1;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), top);
        std::cout << ICON_PROMPT(state) << "  " << label << "\033[K";
        // 重绘 prompt 内容（Yes/No 结果）
        std::cout << "\n"
                  << UTF_VERTICAL_LINE << "  " << ANSI_CANCELLED((choice == Yes ? "Yes" : "No")) << "\n"
                  << UTF_VERTICAL_LINE << "\n"
                  << UTF_CORNER_BOTTOM_LEFT << ANSI_RED("  Exiting.") << "\n\n";
        exit(1); // 立即退出
      }
    }
  }
};

struct Option {
  std::string option, description;
  explicit Option(std::string option, std::string description = "")
      : option(option), description(description) {}
};

struct CLI_PromptSingleSelect : CLI_PROMPT {
  std::string label;
  std::vector<Option> options;
  int selectedIndex = 0;

  CLI_PromptSingleSelect(std::string label, std::vector<Option> opts)
      : label(std::move(label)), options(std::move(opts)) {}

  void prompt(COORD pos) const override {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    for (size_t i = 0; i < options.size(); ++i) {
      COORD line = pos;
      line.Y += i;
      SetConsoleCursorPosition(hConsole, line);
      std::cout << ANSI_BLUE(UTF_VERTICAL_LINE) << "  ";
      if (i == selectedIndex) {
        std::cout << ANSI_GREEN(UTF_RADIO_FILLED) << " " << options[i].option
                  << " " << ANSI_DIM(options[i].description);
      } else {
        std::cout << ANSI_DIM(
            (std::string(UTF_RADIO_EMPTY) + " " + options[i].option));
      }
      std::cout << "\033[K";
    }

    COORD bottom = pos;
    bottom.Y += static_cast<SHORT>(options.size());
    SetConsoleCursorPosition(hConsole, bottom);
    std::cout << ANSI_BLUE(UTF_CORNER_BOTTOM_LEFT) << "\033[K";
    std::cout.flush();
  }

  bool run(bool isLastPrompt) override {
    std::cout << ICON_PROMPT(state) << "  " << label << "\n";
    for (size_t i = 0; i < options.size(); ++i)
      std::cout << UTF_VERTICAL_LINE << "\n";

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    COORD pos = csbi.dwCursorPosition;
    pos.Y -= static_cast<SHORT>(options.size());

    while (true) {
      prompt(pos);
      int c = _getch();
      if (c == 0 || c == 224) {
        int key = _getch();
        if (key == 72 && selectedIndex > 0) // Up arrow
          selectedIndex--;
        else if (key == 80 && selectedIndex < (int)options.size() - 1) // Down
          selectedIndex++;
      } else if (c == 13) { // Enter
        state = PromptState::Successed;

        // 清除所有行
        for (size_t i = 0; i < options.size(); ++i) {
          COORD line = pos;
          line.Y += static_cast<SHORT>(i);
          SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), line);
          std::cout << "\033[K";
        }

        // 重绘 header
        COORD top = pos;
        top.Y -= 1;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), top);
        std::cout << ICON_PROMPT(state) << "  " << label << "\033[K";

        // 输出已选项
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
        std::cout << UTF_VERTICAL_LINE << "  "
                  << ANSI_DIM(options[selectedIndex].option) << "\n";
        std::cout << UTF_VERTICAL_LINE << "\n" << UTF_VERTICAL_LINE << "\n";
        return true;
      } else if (c == 3) {
        state = PromptState::Failed;
        // 清除所有行
        for (size_t i = 0; i < options.size(); ++i) {
          COORD line = pos;
          line.Y += static_cast<SHORT>(i);
          SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), line);
          std::cout << "\033[K";
        }

        // 重绘 header
        COORD top = pos;
        top.Y -= 1;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), top);
        std::cout << ICON_PROMPT(state) << "  " << label << "\033[K";

        // 清除底部行
        COORD bottom = pos;
        bottom.Y += static_cast<SHORT>(options.size());
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), bottom);
        std::cout << "\033[K";

        // 输出已选项
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
        std::cout << UTF_VERTICAL_LINE << "  "
                  << ANSI_CANCELLED(options[selectedIndex].option) << "\n";

        std::cout << UTF_VERTICAL_LINE << "\n"
                  << UTF_CORNER_BOTTOM_LEFT
                  << ANSI_RED("  Operation cancelled.") << "\n\n";

        exit(1);
      }
    }
  }
};

struct CLI_PromptMultiSelect : CLI_PROMPT {
  std::string label;
  std::vector<Option> options;
  std::vector<bool> selected;
  int selectedIndex = 0;

  CLI_PromptMultiSelect(std::string label, std::vector<Option> opts)
      : label(std::move(label)), options(std::move(opts)) {
    selected.resize(options.size(), false);
  }

  void prompt(COORD pos) const override {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    for (size_t i = 0; i < options.size(); ++i) {
      COORD line = pos;
      line.Y += i;
      SetConsoleCursorPosition(hConsole, line);
      std::cout << ANSI_BLUE(UTF_VERTICAL_LINE) << "  ";
      if (i == selectedIndex) {
        std::cout << ICON_RADIO(selected[i]) << " " << options[i].option << " "
                  << ANSI_DIM(options[i].description);
      } else {
        std::cout << ICON_RADIO(selected[i]) << " "
                  << ANSI_DIM(options[i].option);
      }
      std::cout << "\033[K";
    }

    COORD bottom = pos;
    bottom.Y += static_cast<SHORT>(options.size());
    SetConsoleCursorPosition(hConsole, bottom);
    std::cout << ANSI_BLUE(UTF_CORNER_BOTTOM_LEFT) << "\033[K";
    std::cout.flush();
  }

  bool run(bool isLastPrompt) override {
    std::cout << ICON_PROMPT(state) << "  " << label << "\n";
    for (size_t i = 0; i < options.size(); ++i)
      std::cout << UTF_VERTICAL_LINE << "\n";

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    COORD pos = csbi.dwCursorPosition;
    pos.Y -= static_cast<SHORT>(options.size());

    while (true) {
      prompt(pos);
      int c = _getch();
      if (c == 0 || c == 224) {
        int key = _getch();
        if (key == 72 && selectedIndex > 0) // Up arrow
          selectedIndex--;
        else if (key == 80 && selectedIndex < (int)options.size() - 1) // Down
          selectedIndex++;
      } else if (c == 32) { // space toggles selection
        selected[selectedIndex] = !selected[selectedIndex];
      } else if (c == 13) { // Enter
        state = PromptState::Successed;

        // 清除所有行
        for (size_t i = 0; i < options.size(); ++i) {
          COORD line = pos;
          line.Y += static_cast<SHORT>(i);
          SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), line);
          std::cout << "\033[K";
        }
        COORD bottom = pos;
        bottom.Y += static_cast<SHORT>(options.size());
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), bottom);
        std::cout << "\033[K";
        // 重绘 header
        COORD top = pos;
        top.Y -= 1;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), top);
        std::cout << ICON_PROMPT(state) << "  " << label << "\033[K";

        // 输出已选项
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
        std::cout << UTF_VERTICAL_LINE << "  ";
        size_t i = 0;

        while (i < options.size() && !selected[i]) {
          i++;
        }
        if (i < options.size() && selected[i])
          std::cout << ANSI_DIM(options[i].option);
        else
          std::cout << ANSI_DIM("none");
        while (++i < options.size() && selected[i]) {
          {
            std::cout << ANSI_DIM(", " + options[i].option);
          }
        }

        std::cout << "\n";
        //           << ANSI_DIM(options[selectedIndex].option) << "\n";
        std::cout << UTF_VERTICAL_LINE << "\n" << UTF_VERTICAL_LINE << "\n";
        return true;
      } else if (c == 3) {
        state = PromptState::Failed;

        // 清除所有行
        for (size_t i = 0; i < options.size(); ++i) {
          COORD line = pos;
          line.Y += static_cast<SHORT>(i);
          SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), line);
          std::cout << "\033[K";
        }

        // 重绘 header
        COORD top = pos;
        top.Y -= 1;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), top);
        std::cout << ICON_PROMPT(state) << "  " << label << "\033[K";

        COORD bottom = pos;
        bottom.Y += static_cast<SHORT>(options.size());
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), bottom);
        std::cout << "\033[K";

        // 输出已选项
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
        std::cout << UTF_VERTICAL_LINE << "  ";
        size_t i = 0;

        while (i < options.size() && !selected[i]) {
          i++;
        }
        if (i < options.size() && selected[i])
          std::cout << ANSI_STRIKETHROUGH(ANSI_DIM(options[i].option));
        while (++i < options.size() && selected[i]) {
          {
            std::cout << ANSI_DIM(", " + ANSI_STRIKETHROUGH(options[i].option));
          }
        }

        std::cout << "\n"
                  << UTF_VERTICAL_LINE << "\n"
                  << UTF_CORNER_BOTTOM_LEFT
                  << ANSI_RED("  Operation cancelled.") << "\n\n";

        // std::cout << UTF_VERTICAL_LINE << "\n" << UTF_VERTICAL_LINE << "\n";
        exit(1);
      }
    }
  }
};

/* Interactive CLI Runner */
struct Interactive_CLI {
  std::string greeting;
  std::vector<std::shared_ptr<CLI_PROMPT>> prompts;

  Interactive_CLI(std::string greet,
                  std::vector<std::shared_ptr<CLI_PROMPT>> list)
      : greeting(std::move(greet)), prompts(std::move(list)) {}

  void run() {
    setCursorVisible(false);

    std::cout << "\n" << UTF_CORNER_TOP_LEFT << "  " << greeting << "\n";
    std::cout << UTF_VERTICAL_LINE << "\n";

    for (size_t i = 0; i < prompts.size(); ++i) {
      bool isLast = (i == prompts.size() - 1);
      bool ok = prompts[i]->run(isLast);

      // 如果不是最后一个，在下一 prompt 前把上一个 '└' 改为 '│'
      if (!isLast && prompts[i]->state == PromptState::Successed) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        COORD up = csbi.dwCursorPosition;
        up.Y -= 1;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), up);
        // std::cout << UTF_VERTICAL_LINE;
        std::cout.flush();
      } else {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        COORD up = csbi.dwCursorPosition;
        up.Y -= 1;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), up);
        std::cout << UTF_CORNER_BOTTOM_LEFT << "\n";
      }
      // else {
      //   std::cout <<
      // }

      if (!ok)
        break;
    }

    setCursorVisible(true);
  }
};

/* Entry */
int main() {
  Interactive_CLI cli(
      "Welcome to Arch CLI",
      {std::make_shared<CLI_PromptContinue>("Do you want to continue?"),
      std::make_shared<CLI_PromptBoolean>("Yes or No?"),
       std::make_shared<CLI_PromptSingleSelect>(
           "Choose one",
           std::vector<Option>{Option("OptionA", "This is the 1st option"),
                               Option("OptionB", "This is the 2nd option"),
                               Option("OptionC", "This is the  3rd option")}),
       std::make_shared<CLI_PromptMultiSelect>(
           "[3] Select your favorite options:",
           std::vector<Option>{Option("Apples", "Sweet and red."),
                               Option("Bananas", "Good for energy."),
                               Option("Cherries", "Small and juicy.")})

      });
  cli.run();
  return 0;
}
