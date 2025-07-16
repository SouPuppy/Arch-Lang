#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "terminal.h"

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

enum PromptState { Activated, Successed, Failed, Invisible };
enum BooleanChoice { Yes, No };

inline std::string ICON_PROMPT(PromptState state) {
  if (state == PromptState::Activated)
    return ANSI_GREEN(UTF_DIAMOND_FILLED);
  else if (state == PromptState::Successed)
    return ANSI_GREEN(UTF_DIAMOND_EMPTY);
  else
    return ANSI_RED(UTF_BLOCK_FILLED);
}

inline std::string ICON_BOOLEAN(bool selected, const std::string &label) {
  return selected ? (ANSI_GREEN(std::string(UTF_RADIO_FILLED)) + " " + label)
                  : ANSI_DIM(std::string(UTF_RADIO_EMPTY) + " " + label);
}

inline std::string ICON_CHECKBOX(bool selected) {
  return selected ? ANSI_GREEN(UTF_BLOCK_FILLED) : ANSI_GREEN(UTF_BOX_EMPTY);
}

/* Abstract Prompt */
struct CLI_PROMPT {
  PromptState state = PromptState::Activated;
  virtual void prompt(TermCoord pos) const = 0;
  virtual bool run(bool isLastPrompt) = 0;
  virtual ~CLI_PROMPT() = default;
};

/* Yes/No Continue Prompt */
struct CLI_PromptContinue : CLI_PROMPT {
  std::string label;
  BooleanChoice choice = Yes;

  CLI_PromptContinue(std::string text) : label(std::move(text)) {}

  void prompt(TermCoord pos) const override {
    moveCursorTo(pos);

    std::cout << ANSI_BLUE(UTF_VERTICAL_LINE) << "  ";
    std::cout << ICON_BOOLEAN(choice == Yes, "Yes") << ANSI_DIM(" / ")
              << ICON_BOOLEAN(choice == No, "No");
    std::cout << "\n" << ANSI_BLUE(UTF_CORNER_BOTTOM_LEFT);

    std::cout << "\033[K";
    std::cout.flush();
  }

  bool run(bool isLastPrompt) override {
    std::cout << ICON_PROMPT(state) << "  " << label << "\n";
    std::cout << UTF_VERTICAL_LINE << "\n";

    TermCoord pos = getCursorPosition();
    pos.Y -= 1;

    while (true) {
      prompt(pos);

      KeyEvent evt = get_key_event();

      if (evt.key == Key::ArrowLeft) {
        choice = Yes;
      } else if (evt.key == Key::ArrowRight) {
        choice = No;
      } else if (evt.key == Key::Enter) {
        state = (choice == Yes) ? PromptState::Successed : PromptState::Failed;

        // 清除选择和底线行
        clearLineAt(pos);
        TermCoord below = pos;
        below.Y += 1;
        clearLineAt(below);

        // 重绘首行图标
        TermCoord top = pos;
        top.Y -= 1;
        moveCursorTo(top);
        std::cout << ICON_PROMPT(state) << "  " << label << "\033[K";

        // 显示选择结果
        if (choice == Yes) {
          std::cout << "\n"
                    << UTF_VERTICAL_LINE << "  " << ANSI_DIM("Yes") << "\n"
                    << UTF_VERTICAL_LINE << "\n";
        } else {
          std::cout << "\n"
                    << UTF_VERTICAL_LINE << "  " << ANSI_DIM("No") << "\n"
                    << UTF_VERTICAL_LINE << "\n"
                    << UTF_CORNER_BOTTOM_LEFT << ANSI_RED("  Exiting.")
                    << "\n\n";
          setCursorVisible(true);
          exit(0);
        }
        return choice == Yes;

      } else if (evt.key == Key::Escape || (evt.key == Key::CtrlC)) {
        state = PromptState::Failed;

        // 清除选择和底线行
        clearLineAt(pos);
        TermCoord below = pos;
        below.Y += 1;
        clearLineAt(below);

        // 重绘首行图标为失败
        TermCoord top = pos;
        top.Y -= 1;
        moveCursorTo(top);
        std::cout << ICON_PROMPT(state) << "  " << label << "\033[K";

        // 显示取消状态
        std::cout << "\n"
                  << UTF_VERTICAL_LINE << "  "
                  << ANSI_CANCELLED((choice == Yes ? "Yes" : "No")) << "\n"
                  << UTF_VERTICAL_LINE << "\n"
                  << UTF_CORNER_BOTTOM_LEFT << ANSI_RED("  Exiting.") << "\n\n";
        setCursorVisible(true);
        exit(1);
      }
    }
  }
};

/* Yes/No Continue Prompt */
struct CLI_PromptBoolean : CLI_PROMPT {
  std::string label;
  BooleanChoice choice = Yes;

  CLI_PromptBoolean(std::string text) : label(std::move(text)) {}

  void prompt(TermCoord pos) const override {
    moveCursorTo(pos);

    std::cout << ANSI_BLUE(UTF_VERTICAL_LINE) << "  ";
    std::cout << ICON_BOOLEAN(choice == Yes, "Yes") << ANSI_DIM(" / ")
              << ICON_BOOLEAN(choice == No, "No");
    std::cout << "\n" << ANSI_BLUE(UTF_CORNER_BOTTOM_LEFT);

    std::cout << "\033[K";
    std::cout.flush();
  }

  bool run(bool isLastPrompt) override {
    std::cout << ICON_PROMPT(state) << "  " << label << "\n";
    std::cout << UTF_VERTICAL_LINE << "\n";

    TermCoord pos = getCursorPosition();
    pos.Y -= 1;

    while (true) {
      prompt(pos);
      KeyEvent evt = get_key_event();

      switch (evt.key) {
      case Key::ArrowLeft:
      case Key::ArrowUp:
        choice = Yes;
        break;
      case Key::ArrowRight:
      case Key::ArrowDown:
        choice = No;
        break;

      case Key::Enter: {
        state = PromptState::Successed;

        // 清除选择和底线行
        clearLineAt(pos);
        TermCoord below = pos;
        below.Y += 1;
        clearLineAt(below);

        // 重绘首行图标
        TermCoord top = pos;
        top.Y -= 1;
        moveCursorTo(top);
        std::cout << ICON_PROMPT(state) << "  " << label << "\033[K";

        // 显示最终选择
        std::cout << "\n"
                  << UTF_VERTICAL_LINE << "  "
                  << ANSI_DIM((choice == Yes ? "Yes" : "No")) << "\n"
                  << UTF_VERTICAL_LINE << "\n";

        return true;
      }

      case Key::CtrlC: {
        state = PromptState::Failed;

        // 清除选择和底线行
        clearLineAt(pos);
        TermCoord below = pos;
        below.Y += 1;
        clearLineAt(below);

        // 重绘首行图标
        TermCoord top = pos;
        top.Y -= 1;
        moveCursorTo(top);
        std::cout << ICON_PROMPT(state) << "  " << label << "\033[K";

        // 显示取消状态
        std::cout << "\n"
                  << UTF_VERTICAL_LINE << "  "
                  << ANSI_CANCELLED((choice == Yes ? "Yes" : "No")) << "\n"
                  << UTF_VERTICAL_LINE << "\n"
                  << UTF_CORNER_BOTTOM_LEFT
                  << ANSI_RED("  Operation cancelled..") << "\n\n";
        setCursorVisible(true);
        exit(1);
      }

      default:
        break;
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

  void prompt(TermCoord pos) const override {
    for (size_t i = 0; i < options.size(); ++i) {
      TermCoord line = pos;
      line.Y += static_cast<int>(i);
      moveCursorTo(line);

      std::cout << ANSI_BLUE(UTF_VERTICAL_LINE) << "  ";

      if (i == static_cast<size_t>(selectedIndex)) {
        std::cout << ANSI_GREEN(UTF_RADIO_FILLED) << " " << options[i].option
                  << " " << ANSI_DIM(options[i].description);
      } else {
        std::cout << ANSI_DIM(
            (std::string(UTF_RADIO_EMPTY) + " " + options[i].option));
      }

      std::cout << "\033[K"; // 清除剩余行尾
    }

    TermCoord bottom = pos;
    bottom.Y += static_cast<int>(options.size());
    moveCursorTo(bottom);
    std::cout << ANSI_BLUE(UTF_CORNER_BOTTOM_LEFT) << "\033[K";
    std::cout.flush();
  }

  bool run(bool isLastPrompt) override {
    std::cout << ICON_PROMPT(state) << "  " << label << "\n";
    for (size_t i = 0; i < options.size(); ++i)
      std::cout << UTF_VERTICAL_LINE << "\n";

    TermCoord pos = getCursorPosition();
    pos.Y -= static_cast<int>(options.size());

    while (true) {
      prompt(pos);

      KeyEvent evt = get_key_event();
      switch (evt.key) {
      case Key::ArrowLeft:
      case Key::ArrowUp:
        if (selectedIndex > 0)
          selectedIndex--;
        else
          selectedIndex = static_cast<int>(options.size()) - 1;
        break;

      case Key::ArrowRight:
      case Key::ArrowDown:
        if (selectedIndex < static_cast<int>(options.size()) - 1)
          selectedIndex++;
        else
          selectedIndex = 0;
        break;

      case Key::Enter: {
        state = PromptState::Successed;

        // 清除所有选项行
        for (size_t i = 0; i < options.size(); ++i) {
          TermCoord line = pos;
          line.Y += static_cast<int>(i);
          clearLineAt(line);
        }

        // 重绘 header
        TermCoord top = pos;
        top.Y -= 1;
        moveCursorTo(top);
        std::cout << ICON_PROMPT(state) << "  " << label << "\033[K";

        // 输出选中的项
        moveCursorTo(pos);
        std::cout << UTF_VERTICAL_LINE << "  "
                  << ANSI_DIM(options[selectedIndex].option) << "\n";
        std::cout << UTF_VERTICAL_LINE << "\n";
        return true;
      }

      case Key::CtrlC: {
        state = PromptState::Failed;

        // 清除所有选项行
        for (size_t i = 0; i < options.size(); ++i) {
          TermCoord line = pos;
          line.Y += static_cast<int>(i);
          clearLineAt(line);
        }

        // 重绘 header
        TermCoord top = pos;
        top.Y -= 1;
        moveCursorTo(top);
        std::cout << ICON_PROMPT(state) << "  " << label << "\033[K";

        // 清除底部装饰线
        TermCoord bottom = pos;
        bottom.Y += static_cast<int>(options.size());
        clearLineAt(bottom);

        // 输出取消提示
        moveCursorTo(pos);
        std::cout << UTF_VERTICAL_LINE << "  "
                  << ANSI_CANCELLED(options[selectedIndex].option) << "\n";
        std::cout << UTF_VERTICAL_LINE << "\n"
                  << UTF_CORNER_BOTTOM_LEFT
                  << ANSI_RED("  Operation cancelled.") << "\n\n";
        setCursorVisible(true);
        exit(1);
      }

      default:
        break;
      }
    }
  }
};

struct CLI_PromptMultiSelect : CLI_PROMPT {
  bool nullable = true;
  std::string label;
  std::vector<Option> options;
  std::vector<bool> selected;
  int selectedIndex = 0;
  int selectedCount = 0;
  bool warn_no_selection = false;

  CLI_PromptMultiSelect(std::string label, std::vector<Option> opts,
                        bool nullable = true)
      : nullable(nullable), label(std::move(label)), options(std::move(opts)) {
    selected.resize(options.size(), false);
  }

  void prompt(TermCoord pos) const override {
    for (size_t i = 0; i < options.size(); ++i) {
      TermCoord line = pos;
      line.Y += static_cast<int>(i);
      moveCursorTo(line);

      std::cout << (warn_no_selection ? ANSI_YELLOW(UTF_VERTICAL_LINE)
                                      : ANSI_BLUE(UTF_VERTICAL_LINE))
                << "  ";

      if (i == static_cast<size_t>(selectedIndex)) {
        std::cout << ICON_CHECKBOX(selected[i]) << " " << options[i].option;
      } else {
        std::cout << ICON_CHECKBOX(selected[i]) << " "
                  << ANSI_DIM(options[i].option);
      }

      if (selected[i])
        std::cout << " " << ANSI_DIM(options[i].description);

      std::cout << "\033[K"; // 清除行尾
    }

    TermCoord top = pos;
    top.Y -= 1;
    moveCursorTo(top);
    std::cout << (warn_no_selection ? ANSI_YELLOW(UTF_TRIANGLE_UP)
                                    : ICON_PROMPT(state))
              << "  " << label << "\033[K";

    TermCoord bottom = pos;
    bottom.Y += static_cast<int>(options.size());
    moveCursorTo(bottom);
    if (warn_no_selection)
      std::cout << ANSI_YELLOW(UTF_CORNER_BOTTOM_LEFT)
                << ANSI_YELLOW(" Please select at least one option.")
                << "\033[K";
    else
      std::cout << ANSI_BLUE(UTF_CORNER_BOTTOM_LEFT) << "\033[K";
    std::cout.flush();
  }

  bool run(bool isLastPrompt) override {
    std::cout << ICON_PROMPT(state) << "  " << label << "\n";
    for (size_t i = 0; i < options.size(); ++i)
      std::cout << UTF_VERTICAL_LINE << "\n";

    TermCoord pos = getCursorPosition();
    pos.Y -= static_cast<int>(options.size());

    while (true) {
      prompt(pos);

      KeyEvent evt = get_key_event();
      warn_no_selection = false;
      switch (evt.key) {
      case Key::ArrowLeft:
      case Key::ArrowUp:
        if (selectedIndex > 0)
          selectedIndex--;
        else
          selectedIndex = static_cast<int>(options.size()) - 1;
        break;

      case Key::ArrowRight:
      case Key::ArrowDown:
        if (selectedIndex < static_cast<int>(options.size()) - 1)
          selectedIndex++;
        else
          selectedIndex = 0;
        break;

      case Key::Char:
        if (evt.ch == ' ') {
          selected[selectedIndex] = !selected[selectedIndex];
          selectedCount += selected[selectedIndex] ? 1 : -1;
        }
        break;

      case Key::Enter: {
        if (!nullable && selectedCount <= 0) {
          warn_no_selection = true;
          break;
        }

        state = PromptState::Successed;

        // 清除所有选项行
        for (size_t i = 0; i < options.size(); ++i) {
          TermCoord line = pos;
          line.Y += static_cast<int>(i);
          clearLineAt(line);
        }

        // 清除底部提示符行
        TermCoord bottom = pos;
        bottom.Y += static_cast<int>(options.size());
        clearLineAt(bottom);

        // 重绘 header
        TermCoord top = pos;
        top.Y -= 1;
        moveCursorTo(top);
        std::cout << ICON_PROMPT(state) << "  " << label << "\033[K";

        // 输出已选项
        moveCursorTo(pos);
        std::cout << UTF_VERTICAL_LINE << "  ";
        size_t i = 0;
        while (i < options.size() && !selected[i])
          i++;

        if (i < options.size())
          std::cout << ANSI_DIM(options[i].option);
        else
          std::cout << ANSI_DIM("none");

        for (; ++i < options.size();)
          if (selected[i])
            std::cout << ANSI_DIM(", " + options[i].option);

        std::cout << "\n"
                  << UTF_VERTICAL_LINE << "\n"
                  << UTF_VERTICAL_LINE << "\n";
        return true;
      }

      case Key::CtrlC: {
        state = PromptState::Failed;

        // 清除所有选项行
        for (size_t i = 0; i < options.size(); ++i) {
          TermCoord line = pos;
          line.Y += static_cast<int>(i);
          clearLineAt(line);
        }

        // 清除底部提示符行
        TermCoord bottom = pos;
        bottom.Y += static_cast<int>(options.size());
        clearLineAt(bottom);

        // 重绘 header
        TermCoord top = pos;
        top.Y -= 1;
        moveCursorTo(top);
        std::cout << ICON_PROMPT(state) << "  " << label << "\033[K";

        // 输出取消项
        moveCursorTo(pos);
        std::cout << UTF_VERTICAL_LINE << "  ";
        size_t i = 0;
        while (i < options.size() && !selected[i])
          i++;

        if (i < options.size())
          std::cout << ANSI_STRIKETHROUGH(ANSI_DIM(options[i].option));

        bool noSelected = (i == options.size());

        for (; ++i < options.size();)
          if (selected[i])
            std::cout << ANSI_DIM(", " + ANSI_STRIKETHROUGH(options[i].option));

        std::cout << "\n" << (noSelected ? "": (std::string(UTF_VERTICAL_LINE) + "\n"))
                  << UTF_CORNER_BOTTOM_LEFT
                  << ANSI_RED("  Operation cancelled.") << "\n\n";
        setCursorVisible(true);
        exit(1);
      }

      default:
        break;
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

      TermCoord up = getCursorPosition();
      up.Y -= 1;

      moveCursorTo(up);
      if (!isLast && prompts[i]->state == PromptState::Successed) {
        std::cout << UTF_VERTICAL_LINE << "\n";
      } else {
        std::cout << UTF_CORNER_BOTTOM_LEFT << "\n";
      }

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
           "Select your favorite options:",
           std::vector<Option>{Option("Apples", "Sweet and red."),
                               Option("Bananas", "Good for energy."),
                               Option("Cherries", "Small and juicy.")},
           false)

      });
  cli.run();
  return 0;
}
