#ifndef TETRIS_UI_HPP
#define TETRIS_UI_HPP

#include "tetris_game.hpp"

namespace tetris
{
  namespace ui
  {
    /* Enum to identify user game commands. */
    enum class Command
    {
      DO_NOTHING,
      PAUSE,
      QUIT,
      SHIFT_LEFT,
      SHIFT_RIGHT,
      ROTATE_CCW,
      ROTATE_CW,
      SOFT_DROP,
      HARD_DROP,
    };

    /* Convert a Point from playfield coordinates to draw window coordinates */
    game::Point playfield_point_to_draw_window_point(const game::Point& point);

    /* Initialize the ncurses UI */
    void init_ui();

    /* Draw an outline around the playfield */
    void draw_playbox();

    /* Redraw the playfield and then the active tetrimino over it */
    void redraw_playfield(const game::Playfield& playfield, const game::Tetrimino& active_tetrimino);

    /* Redraw the player's current score and level */
    void redraw_score(long score, short level);

    /* Redraw a screen indicating the game is paused */
    void redraw_pause_screen();

    /* Map of (tetrimino type -> ncurses color code) for active and locked minoes */
    const std::map<game::TetriminoType, short> MINO_COLOR{
      {game::TetriminoType::O, 1},
      {game::TetriminoType::I, 2},
      {game::TetriminoType::T, 3},
      {game::TetriminoType::L, 4},
      {game::TetriminoType::J, 5},
      {game::TetriminoType::S, 6},
      {game::TetriminoType::Z, 7},
    };

    /* Map of (tetrimino type -> ncurses color code) for ghost minoes */
    const std::map<game::TetriminoType, short> GHOST_COLOR{
      {game::TetriminoType::O, 8},
      {game::TetriminoType::I, 9},
      {game::TetriminoType::T, 10},
      {game::TetriminoType::L, 11},
      {game::TetriminoType::J, 12},
      {game::TetriminoType::S, 13},
      {game::TetriminoType::Z, 14},
    };
  }
}

#endif
