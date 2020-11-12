#include "tetris.hpp"
#include "tetris_ui.hpp"
#include <locale.h>
#include <ncurses.h>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <map>
#include <random>
#include <sstream>

const std::map<int, Command> INPUT_MAP{
  {ERR, Command::DO_NOTHING},
  {'h', Command::SHIFT_LEFT},
  {'l', Command::SHIFT_RIGHT},
  {'j', Command::ROTATE_CCW},
  {'k', Command::ROTATE_CW},
  {'n', Command::SOFT_DROP},
  {' ', Command::HARD_DROP},
};

int main()
{
  // Initialize ncurses
  initscr();
  curs_set(0);
  cbreak();
  noecho();
  nodelay(stdscr, true);
  keypad(stdscr, true);
  setlocale(LC_ALL, "");

  // Initialize colors, with tetromino types as keys
  start_color();
  use_default_colors();
  init_pair(MINO_COLOR.at(TetriminoType::O), COLOR_WHITE, COLOR_WHITE);
  init_pair(MINO_COLOR.at(TetriminoType::I), COLOR_CYAN, COLOR_CYAN);
  init_pair(MINO_COLOR.at(TetriminoType::T), COLOR_MAGENTA, COLOR_MAGENTA);
  init_pair(MINO_COLOR.at(TetriminoType::L), COLOR_YELLOW, COLOR_YELLOW);
  init_pair(MINO_COLOR.at(TetriminoType::J), COLOR_BLUE, COLOR_BLUE);
  init_pair(MINO_COLOR.at(TetriminoType::S), COLOR_GREEN, COLOR_GREEN);
  init_pair(MINO_COLOR.at(TetriminoType::Z), COLOR_RED, COLOR_RED);
  init_pair(GHOST_COLOR.at(TetriminoType::O), COLOR_WHITE, 0);
  init_pair(GHOST_COLOR.at(TetriminoType::I), COLOR_CYAN, 0);
  init_pair(GHOST_COLOR.at(TetriminoType::T), COLOR_MAGENTA, 0);
  init_pair(GHOST_COLOR.at(TetriminoType::L), COLOR_YELLOW, 0);
  init_pair(GHOST_COLOR.at(TetriminoType::J), COLOR_BLUE, 0);
  init_pair(GHOST_COLOR.at(TetriminoType::S), COLOR_GREEN, 0);
  init_pair(GHOST_COLOR.at(TetriminoType::Z), COLOR_RED, 0);

  draw_playbox();
  refresh();
  getch();

  // Set up game
  Game game;
  game.active_tetrimino = game.bag.pop();
  std::chrono::system_clock::time_point last_tick = std::chrono::system_clock::now();
  std::ofstream logfile {"log.txt"};
  while(true)
  {
    redraw_playfield(game.playfield, game.active_tetrimino);
    auto command = INPUT_MAP.find(getch());
    if (command != INPUT_MAP.end())
      game.try_command(command->second);

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    if (now - last_tick >= game.get_tick_duration())
    {
      if (game.active_tetrimino.is_landed(game.playfield))
      {
        for (const Point& p : game.active_tetrimino.points)
          game.playfield[p] = MINO_COLOR.at(game.active_tetrimino.type);
        game.active_tetrimino = game.bag.pop();
      }
      else
      {
        game.active_tetrimino.fall(game.playfield);
      }
      last_tick = now;
    }
  }

  // Close ncurses window and exit
  endwin();
  return 0;
}
