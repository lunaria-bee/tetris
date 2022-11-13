#include "tetris_cli.hpp"
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


std::ofstream log::out;


int main(int const argc, char* const argv[])
{
  // Open log file
  log::out.open("tetris.log");

  // Set option defaults
  control::GameSettings settings;
  settings.gravity = true;
  settings.preview_size = 6;

  // Process command line options
  cli::opterror cli_errors = cli::process_options(argc, argv, settings);
  if (cli_errors)
  {
    cli::HelpFormatter help(argv[0]);

    if (cli_errors & cli::opterror_flag::BAD_ARG)
      std::cerr << std::endl << help.complete << std::endl;

    else if (cli_errors & cli::opterror_flag::BAD_OPT)
      std::cerr << help.brief << std::endl;

    std::cerr << "Aborting." << std::endl;
    exit(-1);
  }

  log::out << "settings.gravity=" << settings.gravity << std::endl;
  log::out << "settings.preview_size=" << settings.preview_size << std::endl;

  // Initialize UI
  ui::init_ui(settings.preview_size);

  // Set up and play game repeatedly until game-over or user quits
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
