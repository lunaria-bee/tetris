#include "tetris_control.hpp"
#include "tetris_game.hpp"
#include "tetris_ui.hpp"
#include <chrono>
#include <thread>


using namespace tetris;
using namespace tetris::control;


EndType tetris::control::play_game(bool gravity)
{
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
    Command command = Command::DO_NOTHING;
    if (result != INPUT_MAP.end())
      command = result->second;

    // Quit early if needed
    if (command == Command::QUIT)
      return EndType::QUIT;
    if (command == Command::RESTART)
      return EndType::RESTART;

    if (!paused)
    {
      if (!gravity
          || !extended_placement_active
          || extended_placement_moves <= EXTENDED_PLACEMENT_MAX_MOVES)
      {
        Command command = result->second;

        bool move_executed = false;
        switch (command)
        {
          case Command::DO_NOTHING:
            break;

          case Command::PAUSE:
            paused = true;

          case Command::SHIFT_LEFT:
            move_executed = game.active_tetrimino.translate(game::Point(0, -1), game.playfield);
            break;

          case Command::SHIFT_RIGHT:
            move_executed = game.active_tetrimino.translate(game::Point(0, 1), game.playfield);
            break;

          case Command::ROTATE_CCW:
            move_executed = game.active_tetrimino.rotate_ccw(game.playfield);
            break;

          case Command::ROTATE_CW:
            move_executed = game.active_tetrimino.rotate_cw(game.playfield);
            break;

          case Command::SOFT_DROP:
            move_executed = game.active_tetrimino.translate(game::Point(1, 0), game.playfield);
            if (move_executed)
              last_drop = tick_start;
            break;

          case Command::HARD_DROP:
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
            || gravity && tick_start > extended_placement_start + EXTENDED_PLACEMENT_MAX_TIME)
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
    else if (command == Command::PAUSE)
    {
      paused = false;
    }

    // Delay until next tick
    tick_end = std::chrono::steady_clock::now();
    std::chrono::duration<float> work_time = tick_end - tick_start;
    if (work_time < TICK_DURATION)
      std::this_thread::sleep_for(TICK_DURATION - work_time);
  }

  return EndType::GAME_OVER;
}
