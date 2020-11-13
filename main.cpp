#include "tetris.hpp"
#include "tetris_log.hpp"
#include "tetris_ui.hpp"
#include <locale.h>
#include <ncurses.h>
#include <chrono>
#include <fstream>
#include <map>
#include <thread>

std::ofstream tetris_log::out;

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
  tetris_log::out.open("tetris.log");

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
  std::chrono::steady_clock::time_point last_drop = std::chrono::steady_clock::now();
  bool extended_placement_active = false;
  short extended_placement_moves;
  std::chrono::steady_clock::time_point extended_placement_start;
  std::chrono::steady_clock::time_point tick_start = std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point tick_end = std::chrono::steady_clock::now();

  while (true)
  {
    tick_start = std::chrono::steady_clock::now();

    redraw_playfield(game.playfield, game.active_tetrimino);

    // Process input
    auto result = INPUT_MAP.find(getch());
    if (result != INPUT_MAP.end())
    {
      if (!extended_placement_active || extended_placement_moves <= extended_placement_max_moves)
      {
        Command command = result->second;
        bool command_success = game.try_command(result->second);
        if (command_success)
        {
          if (command == Command::SOFT_DROP)
            last_drop = tick_start;

          if (extended_placement_active)
          {
            extended_placement_start = tick_start;
            ++extended_placement_moves;
          }
        }
      }
    }

    // Process drop
    if (tick_start - last_drop >= game.get_drop_interval())
    {
      bool fell = game.active_tetrimino.fall(game.playfield);
      if (fell && extended_placement_active)
        extended_placement_active = false;
      last_drop = tick_start;
    }

    // Check for mino landing
    if (game.active_tetrimino.is_landed(game.playfield))
    {
      if (!extended_placement_active)
      {
        extended_placement_start = tick_start;
        extended_placement_moves = 0;
        extended_placement_active = true;
      }

      if (tick_start > extended_placement_start + extended_placement_max_time)
      {
        for (const Point& p : game.active_tetrimino.points)
          game.playfield[p] = MINO_COLOR.at(game.active_tetrimino.type);
        game.active_tetrimino = game.bag.pop();

        extended_placement_active = false;
      }
    }

    // Delay until next tick
    tick_end = std::chrono::steady_clock::now();
    std::chrono::duration<float> work_time = tick_end - tick_start;
    if (work_time < tick_duration)
      std::this_thread::sleep_for(tick_duration - work_time);
  }

  // Close ncurses window and exit
  endwin();
  return 0;
}
