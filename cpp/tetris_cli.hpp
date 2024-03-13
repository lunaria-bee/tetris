#ifndef TETRIS_CLI_HPP
#define TETRIS_CLI_HPP

#include "tetris_control.hpp"
#include <getopt.h>
#include <string>

// TODO resize UI when terminal is resized

namespace tetris
{
  namespace cli
  {
    using opterror = int;
    namespace opterror_flag
    {
      const int NONE    = 0x0;
      const int BAD_OPT = 0x1;
      const int BAD_ARG = 0x1<<1;
    }

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

    opterror process_options(int const argc, char* const argv[], control::GameSettings& settings);
  }
}

#endif
