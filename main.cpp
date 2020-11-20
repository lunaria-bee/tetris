#include "tetris.hpp"
#include "tetris_log.hpp"
#include "tetris_ui.hpp"
#include <locale.h>
#include <ncurses.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <thread>

bool gravity=true; // TODO set from command line / config file

std::ofstream tetris::log::out;

const std::map<int, tetris::ui::Command> INPUT_MAP{
  {ERR, tetris::ui::Command::DO_NOTHING},
  {'p', tetris::ui::Command::PAUSE},
  {'q', tetris::ui::Command::QUIT},
  {'h', tetris::ui::Command::SHIFT_LEFT},
  {'l', tetris::ui::Command::SHIFT_RIGHT},
  {'j', tetris::ui::Command::ROTATE_CCW},
  {'k', tetris::ui::Command::ROTATE_CW},
  {'n', tetris::ui::Command::SOFT_DROP},
  {' ', tetris::ui::Command::HARD_DROP},
};

int main()
{
  tetris::log::out.open("tetris.log");

  tetris::ui::init_ui();

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
  bool paused = false;

  while (!game.is_game_over())
  {
    tick_start = std::chrono::steady_clock::now();

    if (paused)
      tetris::ui::redraw_pause_screen();
    else
      tetris::ui::redraw_playfield(game.playfield, game.active_tetrimino);

    tetris::ui::redraw_score(game.score, game.level);

    // Get input
    auto result = INPUT_MAP.find(getch());
    tetris::ui::Command command = tetris::ui::Command::DO_NOTHING;
    if (result != INPUT_MAP.end())
      command = result->second;

    if (command == tetris::ui::Command::QUIT)
      break;

    if (!paused)
    {
      if (!gravity
          || !extended_placement_active
          || extended_placement_moves <= tetris::EXTENDED_PLACEMENT_MAX_MOVES)
      {
        tetris::ui::Command command = result->second;

        bool move_executed = false;
        switch (command)
        {
          case tetris::ui::Command::DO_NOTHING:
            break;

          case tetris::ui::Command::PAUSE:
            paused = true;

          case tetris::ui::Command::SHIFT_LEFT:
            move_executed = game.active_tetrimino.translate(tetris::Point(0, -1), game.playfield);
            break;

          case tetris::ui::Command::SHIFT_RIGHT:
            move_executed = game.active_tetrimino.translate(tetris::Point(0, 1), game.playfield);
            break;

          case tetris::ui::Command::ROTATE_CCW:
            move_executed = game.active_tetrimino.rotate_ccw(game.playfield);
            break;

          case tetris::ui::Command::ROTATE_CW:
            move_executed = game.active_tetrimino.rotate_cw(game.playfield);
            break;

          case tetris::ui::Command::SOFT_DROP:
            move_executed = game.active_tetrimino.translate(tetris::Point(1, 0), game.playfield);
            if (move_executed)
              last_drop = tick_start;
            break;

          case tetris::ui::Command::HARD_DROP:
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
          bool fell = game.active_tetrimino.translate(tetris::Point(1, 0), game.playfield);
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
            || gravity && tick_start > extended_placement_start + tetris::EXTENDED_PLACEMENT_MAX_TIME)
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
    else if (command == tetris::ui::Command::PAUSE)
    {
      paused = false;
    }

    // Delay until next tick
    tick_end = std::chrono::steady_clock::now();
    std::chrono::duration<float> work_time = tick_end - tick_start;
    if (work_time < tetris::TICK_DURATION)
      std::this_thread::sleep_for(tetris::TICK_DURATION - work_time);
  }

  // Close ncurses window and exit
  endwin();
  std::cout << "Game over!" << std::endl;
  std::cout << "Score: " << game.score << std::endl;
  return 0;
}
