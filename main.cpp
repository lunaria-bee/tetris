#include "tetris_game.hpp"
#include "tetris_log.hpp"
#include "tetris_ui.hpp"
#include <locale.h>
#include <ncurses.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <thread>


using namespace tetris;


bool gravity=true; // TODO set from command line / config file

std::ofstream log::out;

const std::map<int, ui::Command> INPUT_MAP{
  {ERR, ui::Command::DO_NOTHING},
  {'p', ui::Command::PAUSE},
  {'q', ui::Command::QUIT},
  {'h', ui::Command::SHIFT_LEFT},
  {'l', ui::Command::SHIFT_RIGHT},
  {'j', ui::Command::ROTATE_CCW},
  {'k', ui::Command::ROTATE_CW},
  {'n', ui::Command::SOFT_DROP},
  {' ', ui::Command::HARD_DROP},
};


int main()
{
  log::out.open("tetris.log");

  ui::init_ui();

  // Set up game
  game::Game game;
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
  bool paused = false;

  while (!game.is_game_over())
  {
    tick_start = std::chrono::steady_clock::now();

    if (paused)
      ui::redraw_pause_screen();
    else
      ui::redraw_playfield(game.playfield, game.active_tetrimino);

    ui::redraw_score(game.score, game.level);

    // Get input
    auto result = INPUT_MAP.find(getch());
    ui::Command command = ui::Command::DO_NOTHING;
    if (result != INPUT_MAP.end())
      command = result->second;

    if (command == ui::Command::QUIT)
      break;

    if (!paused)
    {
      if (!gravity
          || !extended_placement_active
          || extended_placement_moves <= game::EXTENDED_PLACEMENT_MAX_MOVES)
      {
        ui::Command command = result->second;

        bool move_executed = false;
        switch (command)
        {
          case ui::Command::DO_NOTHING:
            break;

          case ui::Command::PAUSE:
            paused = true;

          case ui::Command::SHIFT_LEFT:
            move_executed = game.active_tetrimino.translate(game::Point(0, -1), game.playfield);
            break;

          case ui::Command::SHIFT_RIGHT:
            move_executed = game.active_tetrimino.translate(game::Point(0, 1), game.playfield);
            break;

          case ui::Command::ROTATE_CCW:
            move_executed = game.active_tetrimino.rotate_ccw(game.playfield);
            break;

          case ui::Command::ROTATE_CW:
            move_executed = game.active_tetrimino.rotate_cw(game.playfield);
            break;

          case ui::Command::SOFT_DROP:
            move_executed = game.active_tetrimino.translate(game::Point(1, 0), game.playfield);
            if (move_executed)
              last_drop = tick_start;
            break;

          case ui::Command::HARD_DROP:
            move_executed = game.active_tetrimino.hard_drop(game.playfield);
            if (move_executed)
              hard_drop = true;
            break;
        }

        if (move_executed && extended_placement_active)
        {
          extended_placement_start = tick_start;
          ++extended_placement_moves;
        }
      }

      // Process drop
      if (gravity)
      {
        if (tick_start - last_drop >= game.get_drop_interval())
        {
          bool fell = game.active_tetrimino.translate(game::Point(1, 0), game.playfield);
          if (fell && extended_placement_active)
            extended_placement_active = false;
          last_drop = tick_start;
        }
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
        if (hard_drop
            || gravity && tick_start > extended_placement_start + game::EXTENDED_PLACEMENT_MAX_TIME)
        {
          game.lock_active_tetrimino();
          game.clear_rows();
          game.draw_new_tetrimino();

          // Reset placement control
          extended_placement_active = false;
          hard_drop = false;
        }
      }
    }
    else if (command == ui::Command::PAUSE)
    {
      paused = false;
    }

    // Delay until next tick
    tick_end = std::chrono::steady_clock::now();
    std::chrono::duration<float> work_time = tick_end - tick_start;
    if (work_time < game::TICK_DURATION)
      std::this_thread::sleep_for(game::TICK_DURATION - work_time);
  }

  // Close ncurses window and exit
  endwin();
  std::cout << "Game over!" << std::endl;
  std::cout << "Score: " << game.score << std::endl;
  return 0;
}
