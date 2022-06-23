#ifndef TETRIS_CLI_HPP
#define TETRIS_CLI_HPP

#include "tetris_control.hpp"
#include <getopt.h>
#include <string>

namespace tetris
{
  namespace cli
  {
    const char OPTSTRING[5] = "p:Gh";
    const option LONGOPTS[4] = {
      {"preview-size", true, nullptr, 'p'},
      {"disable-gravity", false, nullptr, 256},
      {"help", false, nullptr, 1024},
      {0, 0, 0, 0},
    };

    struct HelpFormatter
    {
      std::string usage;
      std::string details;
      std::string brief;
      std::string complete;

      HelpFormatter(const std::string& run_command);
    };

    bool process_options(int const argc, char* const argv[], control::GameSettings& settings);
  }
}

#endif
