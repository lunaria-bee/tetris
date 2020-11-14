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

const std::map<int, tetris::Command> INPUT_MAP{
  {ERR, tetris::Command::DO_NOTHING},
  {'h', tetris::Command::SHIFT_LEFT},
  {'l', tetris::Command::SHIFT_RIGHT},
  {'j', tetris::Command::ROTATE_CCW},
  {'k', tetris::Command::ROTATE_CW},
  {'n', tetris::Command::SOFT_DROP},
  {' ', tetris::Command::HARD_DROP},
};

int main()
{
  tetris_log::out.open("tetris.log");

  tetris_ui::init_ui();

  // Set up game
  tetris::Game game;
  game.draw_new_tetrimino();

  // Set up placement control
  bool extended_placement_active = false;
  std::chrono::steady_clock::time_point extended_placement_start;
  short extended_placement_moves;
  bool hard_drop = false;

  // Set up time control
  std::chrono::steady_clock::time_point last_drop = std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point tick_start = std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point tick_end = std::chrono::steady_clock::now();

  while (!game.is_game_over())
  {
    tick_start = std::chrono::steady_clock::now();

    tetris_ui::redraw_playfield(game.playfield, game.active_tetrimino);

    // Process input
    auto result = INPUT_MAP.find(getch());
    if (result != INPUT_MAP.end())
    {
      if (!extended_placement_active || extended_placement_moves <= tetris::EXTENDED_PLACEMENT_MAX_MOVES)
      {
        tetris::Command command = result->second;
        bool command_success = game.try_command(result->second);
        if (command_success)
        {
          if (command == tetris::Command::SOFT_DROP)
            last_drop = tick_start;

          if (command == tetris::Command::HARD_DROP)
            hard_drop = true;

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
      bool fell = game.active_tetrimino.translate(tetris::Point(1, 0), game.playfield);
      if (fell && extended_placement_active)
        extended_placement_active = false;
      last_drop = tick_start;
    }

    // Check for mino landing
    if (game.active_tetrimino.is_landed(game.playfield))
    {
      // Initialize extended placement mode
      if (!extended_placement_active)
      {
        extended_placement_start = tick_start;
        extended_placement_moves = 0;
        extended_placement_active = true;
      }

      // If tetrimino may no longer be manipulated
      if (hard_drop || tick_start > extended_placement_start + tetris::EXTENDED_PLACEMENT_MAX_TIME)
      {
        game.lock_active_tetrimino();
        game.clear_rows();
        game.draw_new_tetrimino();

        // Reset placement control
        extended_placement_active = false;
        hard_drop = false;
      }
    }

    // Delay until next tick
    tick_end = std::chrono::steady_clock::now();
    std::chrono::duration<float> work_time = tick_end - tick_start;
    if (work_time < tetris::TICK_DURATION)
      std::this_thread::sleep_for(tetris::TICK_DURATION - work_time);
  }

  // Close ncurses window and exit
  endwin();
  return 0;
}
