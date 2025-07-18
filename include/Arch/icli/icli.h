#pragma once

#include "arch/icli/terminal_utils.h"
#include <memory>
#include <vector>
#include <functional>

enum PromptState { Activated, Succeed, Failed, Invisible };
enum BooleanChoice { Yes, No };
struct Option {
  std::string option, description;
  explicit Option(std::string option, std::string description = "")
      : option(option), description(description) {}
};

/* Abstract Prompt */
struct CLI_PROMPT {
  std::function<bool()> precondition = [] { return true; };
  std::function<void()> action = [] {};
  PromptState state = PromptState::Activated;
  virtual void prompt(TermCoord pos) const = 0;
  virtual bool run(bool isLastPrompt) = 0;
  virtual ~CLI_PROMPT() = default;
};

/* Execute a command */
struct CLI_Execute : CLI_PROMPT {
  explicit CLI_Execute(
    std::function<bool()> precond = [] { return true; },
    std::function<void()> act = [] {}
  ) {
    /* this is a special prompt that will never be shown (only executable) */
    state = Invisible;
    precondition = std::move(precond);
    action = std::move(act);
  }
  void prompt(TermCoord pos) const override {};
  bool run(bool isLastPrompt) override { return true; };
};

/* Yes/No Continue Prompt */
struct CLI_PromptContinue : CLI_PROMPT {
  std::string label;
  BooleanChoice choice = Yes;

  explicit CLI_PromptContinue(
    std::string text,
    std::function<bool()> precond = [] { return true; },
    std::function<void()> act = [] {}
  ) : label(std::move(text)) {
    precondition = std::move(precond);
    action = std::move(act);
  }

  void prompt(TermCoord pos) const override;
  bool run(bool isLastPrompt) override;
};

struct CLI_PromptInput : CLI_PROMPT {
  std::string label;
  std::string input;
  std::string fallback;
  bool warn_need_input = false;

  explicit CLI_PromptInput(std::string text, std::string fallback="") : label(std::move(text)), fallback(fallback){}

  void prompt(TermCoord pos) const override;

  bool run(bool isLastPrompt) override;
};

/* Yes/No Continue Prompt */
struct CLI_PromptBoolean : CLI_PROMPT {
  std::string label;
  BooleanChoice choice = Yes;

  explicit CLI_PromptBoolean(std::string text) : label(std::move(text)) {}

  void prompt(TermCoord pos) const override;

  bool run(bool isLastPrompt) override;
};

struct CLI_PromptSingleSelect : CLI_PROMPT {
  std::string label;
  std::vector<Option> options;
  int selectedIndex = 0;

  CLI_PromptSingleSelect(std::string label, std::vector<Option> opts)
      : label(std::move(label)), options(std::move(opts)) {}

  void prompt(TermCoord pos) const override;

  bool run(bool isLastPrompt) override;
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

  void prompt(TermCoord pos) const override;

  bool run(bool isLastPrompt) override;
};

/* Interactive CLI Runner */
struct Interactive_CLI {
  std::string greeting;
  std::vector<std::shared_ptr<CLI_PROMPT>> prompts;

  Interactive_CLI(std::string greet,
                  std::vector<std::shared_ptr<CLI_PROMPT>> list)
      : greeting(std::move(greet)), prompts(std::move(list)) {}

  void run();
};
