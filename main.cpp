#include "tetris_control.hpp"
#include "tetris_game.hpp"
#include "tetris_log.hpp"
#include "tetris_ui.hpp"
#include <getopt.h>
#include <locale.h>
#include <ncurses.h>
#include <fstream>
#include <iostream>
#include <string>


using namespace tetris;


namespace tetris
{
  namespace options
  {
    const char OPTSTRING[5] = "hGp:";
    const option LONGOPTS[2] = {
      {"help", false, nullptr, 256},
      {0, 0, 0, 0},
    };
  }

  namespace help
  {
    std::string usage;
    std::string details;
    std::string brief;
    std::string complete;

    void init(const std::string& run_command)
    {
      usage =
        "Usage: " + run_command + " [-G] [-p PREVIEW_SIZE]";

      details =
        "-G               Disable gravity. Pieces will not fall unless soft dropped or " "\n"
        "                 hard dropped, and must be hard dropped to lock in place." "\n"
        "-p PREVIEW_SIZE  Set the number of tetriminoes to show in the piece preview.";

      brief =
        usage + "\n"
        + "Try '" + run_command + " --help' for more inforation.";

      complete =
        usage +  "\n"
        + "\n"
        + details;
    }
  }
}


std::ofstream log::out;

sss
int main(int const argc, char* const argv[])
{
  control::GameSettings settings;
  settings.gravity = true;
  settings.preview_size = 6;

  int opt;
  int* longindex = nullptr;
  while ((opt = getopt_long(argc, argv, options::OPTSTRING, options::LONGOPTS, longindex)) != -1)
  {
    switch(opt)
j    {
      case 'G':
        settings.gravity = false;
        break;

      case 'p':
      {
        int preview_size = atoi(optarg);
        if (preview_size > 6)
        {
          std::cerr << "Error: "
                    << "Maximum preview size is 6 ("
                    << preview_size
                    << " attempted)."
                    << std::endl
                    << "Aborting."
                    << std::endl;
          exit(-1);
        }
        settings.preview_size = preview_size;
        break;
      }

      case 'h':
        tetris::help::init(argv[0]);
        std::cout << help::brief << std::endl;
        exit(0);
        break;

      case 256: // --help
        tetris::help::init(argv[0]);
        std:: cout << help::complete << std::endl;
        exit(0);
        break;

      case '?':
        tetris::help::init(argv[0]);
        std::cout << help::brief << std::endl;
        exit(-1);
        break;

      default:
        break;
    }
  }

  log::out.open("tetris.log");

  ui::init_ui();

  control::GameResult result;
  bool play = true;
  while (play)
  {
    result = control::play_game(settings);
    switch (result.end_type)
    {
      case control::EndType::GAME_OVER:
      case control::EndType::QUIT:
        play = false;
        break;

      case control::EndType::RESTART:
        break;
    }
  }

  // Close ncurses window and exit
  endwin();
  std::cout << "Game over!" << std::endl;
  std::cout << "Score: " << result.end_score << std::endl;
  return 0;
}
