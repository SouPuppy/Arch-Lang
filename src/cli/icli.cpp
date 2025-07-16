#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "Arch/icli/terminal_utils.h"
#include "Arch/icli.h"

inline std::string ICON_PROMPT(const PromptState state) {
  if (state == PromptState::Activated)
    return ANSI_GREEN(UTF_DIAMOND_FILLED);
  else if (state == PromptState::Succeed)
    return ANSI_GREEN(UTF_DIAMOND_EMPTY);
  else
    return ANSI_RED(UTF_BLOCK_FILLED);
}

inline std::string ICON_BOOLEAN(const bool selected, const std::string &label) {
  return selected ? (ANSI_GREEN(std::string(UTF_RADIO_FILLED)) + " " + label)
                  : ANSI_DIM(std::string(UTF_RADIO_EMPTY) + " " + label);
}

inline std::string ICON_CHECKBOX(const bool selected) {
  return selected ? ANSI_GREEN(UTF_BLOCK_FILLED) : ANSI_GREEN(UTF_BOX_EMPTY);
}

void CLI_PromptContinue::prompt(const TermCoord pos) const {
  moveCursorTo(pos);

  std::cout << ANSI_BLUE(UTF_VERTICAL_LINE) << "  ";
  std::cout << ICON_BOOLEAN(choice == Yes, "Yes") << ANSI_DIM(" / ")
      << ICON_BOOLEAN(choice == No, "No");
  std::cout << "\n" << ANSI_BLUE(UTF_CORNER_BOTTOM_LEFT);

  std::cout << "\033[K";
  std::cout.flush();
}

bool CLI_PromptContinue::run(bool isLastPrompt) {
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
      state = (choice == Yes) ? PromptState::Succeed : PromptState::Failed;

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

void CLI_PromptInput::prompt(TermCoord pos) const {
  moveCursorTo(pos);
  std::string display = input;
  if (display.empty() && !fallback.empty()) {
    // 第一个字符反转模拟光标，其余字符淡化显示
    std::string head = std::string("\033[7m") + fallback[0] + "\033[0m";
    std::string tail = fallback.substr(1);
    display = head + ANSI_DIM(tail);
  }
  else
    display += "\033[7m \033[0m"; // Psuedo-cursor effect
  std::cout << (warn_need_input ? ANSI_YELLOW(UTF_VERTICAL_LINE) : ANSI_BLUE(UTF_VERTICAL_LINE)) << "  " << display << "\033[K";


  TermCoord top = pos;
  top.Y -= 1;
  moveCursorTo(top);
  std::cout << (warn_need_input ? ANSI_YELLOW(UTF_TRIANGLE_UP)
                  : ICON_PROMPT(state))
      << "  " << label << "\033[K";

  moveCursorTo(addY(pos, 1));
  if (warn_need_input) {
    std::cout << ANSI_YELLOW(UTF_CORNER_BOTTOM_LEFT)
        << ANSI_YELLOW("  Value cannot be empty.") << "\033[K";
  } else {
    std::cout << ANSI_BLUE(UTF_CORNER_BOTTOM_LEFT) << "\033[K";
  }
  std::cout.flush();
}

bool CLI_PromptInput::run(bool isLastPrompt) {
  std::cout << ICON_PROMPT(state) << "  " << label << "\n";
  std::cout << UTF_VERTICAL_LINE << "\n";

  TermCoord inputLine = addY(getCursorPosition(), -1);  // 输入行位置

  while (true) {
    prompt(inputLine);  // 显示模拟光标
    warn_need_input = false;
    KeyEvent evt = get_key_event();

    switch (evt.key) {
      case Key::Char:
        if (evt.ch >= 32 && evt.ch <= 126)
          input.push_back(evt.ch);
        break;

      case Key::Backspace:
        if (!input.empty())
          input.pop_back();
        break;

      case Key::Enter: {
        if (input.empty()) {
          if (fallback.empty()) {
            warn_need_input = true;
            break;
          }
          input = fallback;
        }
        state = PromptState::Succeed;

        clearLineAt(inputLine);
        clearLineAt(addY(inputLine, 1));

        moveCursorTo(addY(inputLine, -1));
        std::cout << ICON_PROMPT(state) << "  " << label << "\033[K";

        std::cout << "\n" << UTF_VERTICAL_LINE << "  " << ANSI_DIM(input)
            << "\n" << UTF_VERTICAL_LINE << "\n";
        return true;
      }

      case Key::Escape:
      case Key::CtrlC: {
        state = PromptState::Failed;

        clearLineAt(inputLine);
        clearLineAt(addY(inputLine, 1));

        moveCursorTo(addY(inputLine, -1));
        std::cout << ICON_PROMPT(state) << "  " << label << "\033[K";

        if (!input.empty())
          std::cout << "\n" << UTF_VERTICAL_LINE << "  " << ANSI_CANCELLED(input);

        std::cout << "\n" << UTF_VERTICAL_LINE << "\n"
            << UTF_CORNER_BOTTOM_LEFT << ANSI_RED("  Operation cancelled.")
            << "\n\n";
        setCursorVisible(true);
        exit(1);
      }

      default:
        break;
    }
  }
}

void CLI_PromptBoolean::prompt(TermCoord pos) const {
  moveCursorTo(pos);

  std::cout << ANSI_BLUE(UTF_VERTICAL_LINE) << "  ";
  std::cout << ICON_BOOLEAN(choice == Yes, "Yes") << ANSI_DIM(" / ")
      << ICON_BOOLEAN(choice == No, "No");
  std::cout << "\n" << ANSI_BLUE(UTF_CORNER_BOTTOM_LEFT);

  std::cout << "\033[K";
  std::cout.flush();
}

bool CLI_PromptBoolean::run(bool isLastPrompt) {
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
        state = PromptState::Succeed;

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





void CLI_PromptSingleSelect::prompt(TermCoord pos) const {
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

bool CLI_PromptSingleSelect::run(bool isLastPrompt) {
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
        state = PromptState::Succeed;

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



void CLI_PromptMultiSelect::prompt(TermCoord pos) const {
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
        << ANSI_YELLOW("  Please select at least one option.")
        << "\033[K";
  else
    std::cout << ANSI_BLUE(UTF_CORNER_BOTTOM_LEFT) << "\033[K";
  std::cout.flush();
}

bool CLI_PromptMultiSelect::run(bool isLastPrompt) {
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

        state = PromptState::Succeed;

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

void Interactive_CLI::run() {
  setCursorVisible(false);

  std::cout << "\n" << UTF_CORNER_TOP_LEFT << "  " << greeting << "\n";
  std::cout << UTF_VERTICAL_LINE << "\n";

  for (size_t i = 0; i < prompts.size(); ++i) {
    bool isLast = (i == prompts.size() - 1);
    bool ok = prompts[i]->run(isLast);

    TermCoord up = getCursorPosition();
    up.Y -= 1;

    moveCursorTo(up);
    if (!isLast && prompts[i]->state == PromptState::Succeed) {
      std::cout << UTF_VERTICAL_LINE << "\n";
    } else {
      std::cout << UTF_CORNER_BOTTOM_LEFT << "\n";
    }

    if (!ok)
      break;
  }

  setCursorVisible(true);
}
