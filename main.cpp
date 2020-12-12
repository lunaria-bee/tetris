#include "tetris_control.hpp"
#include "tetris_game.hpp"
#include "tetris_log.hpp"
#include "tetris_ui.hpp"
#include <locale.h>
#include <ncurses.h>
#include <fstream>
#include <iostream>


using namespace tetris;


std::ofstream log::out;


int main()
{
  log::out.open("tetris.log");

  ui::init_ui();

  bool play = true;
  while (play)
  {
    control::EndType result = control::play_game();
    switch (result)
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
  // std::cout << "Game over!" << std::endl;
  // std::cout << "Score: " << game.score << std::endl;
  return 0;
}
