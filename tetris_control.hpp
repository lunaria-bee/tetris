#ifndef TETRIS_CONTROL_HPP
#define TETRIS_CONTROL_HPP

#include "tetris_game.hpp"
#include <ncurses.h>
#include <chrono>

namespace tetris
{
  namespace control
  {
    /* Enum to identify user game commands. */
    enum class Command
    {
      DO_NOTHING,
      PAUSE,
      QUIT,
      RESTART,
      SHIFT_LEFT,
      SHIFT_RIGHT,
      ROTATE_CCW,
      ROTATE_CW,
      SOFT_DROP,
      HARD_DROP,
    };

    /* Enum to identify how a game ended */
    enum class EndType
    {
      GAME_OVER,
      QUIT,
      RESTART,
    };

    /* Struct for game settings */
    struct GameSettings
    {
      bool gravity;
      short preview_size;
    };

    /* Struct for all results of a game */
    struct GameResult
    {
      EndType end_type;
      short end_level;
      long end_score;

      GameResult();

      GameResult(EndType end_type_init, short end_level_init, long end_score_init);
    };

    /* Map of (char -> command) */
    const std::map<int, Command> INPUT_MAP{
      {ERR, Command::DO_NOTHING},
      {'p', Command::PAUSE},
      {'q', Command::QUIT},
      {'r', Command::RESTART},
      {'h', Command::SHIFT_LEFT},
      {'l', Command::SHIFT_RIGHT},
      {'j', Command::ROTATE_CCW},
      {'k', Command::ROTATE_CW},
      {'n', Command::SOFT_DROP},
      {' ', Command::HARD_DROP},
    };

    /* Length of each game tick. */
    const std::chrono::duration<float> TICK_DURATION(1.0/60.0);

    /* Maximum number of moves permitted in extended placement mode. */
    const short EXTENDED_PLACEMENT_MAX_MOVES = 15;

    /* Extended placement timer duration. */
    const std::chrono::duration<float> EXTENDED_PLACEMENT_MAX_TIME(0.5);

    /* Play a game of tetris */
    GameResult play_game(GameSettings settings);

    /* Handle game over */
    bool handle_game_over();
  }
}

#endif
