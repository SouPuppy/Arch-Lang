#include <vector>
#include <memory>
#include "Arch/icli.h"

int main() {
  Interactive_CLI cli(
      "Welcome to Arch CLI",
      {
       std::make_shared<CLI_PromptInput>("Enter your name:"),
       std::make_shared<CLI_PromptInput>("Enter your name:", "default"),
       std::make_shared<CLI_PromptContinue>("Do you want to continue?"),
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
