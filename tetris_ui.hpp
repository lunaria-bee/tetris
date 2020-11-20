#ifndef TETRIS_UI_HPP
#define TETRIS_UI_HPP

#include "tetris.hpp"

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
    tetris::Point playfield_point_to_draw_window_point(const tetris::Point& point);

    /* Initialize the ncurses UI */
    void init_ui();

    /* Draw an outline around the playfield */
    void draw_playbox();

    /* Redraw the playfield and then the active tetrimino over it */
    void redraw_playfield(const tetris::Playfield& playfield, const tetris::Tetrimino& active_tetrimino);

    /* Redraw the player's current score and level */
    void redraw_score(long score, short level);

    /* Redraw a screen indicating the game is paused */
    void redraw_pause_screen();

    /* Map of (tetrimino type -> ncurses color code) for active and locked minoes */
    const std::map<tetris::TetriminoType, short> MINO_COLOR{
      {tetris::TetriminoType::O, 1},
      {tetris::TetriminoType::I, 2},
      {tetris::TetriminoType::T, 3},
      {tetris::TetriminoType::L, 4},
      {tetris::TetriminoType::J, 5},
      {tetris::TetriminoType::S, 6},
      {tetris::TetriminoType::Z, 7},
    };

    /* Map of (tetrimino type -> ncurses color code) for ghost minoes */
    const std::map<tetris::TetriminoType, short> GHOST_COLOR{
      {tetris::TetriminoType::O, 8},
      {tetris::TetriminoType::I, 9},
      {tetris::TetriminoType::T, 10},
      {tetris::TetriminoType::L, 11},
      {tetris::TetriminoType::J, 12},
      {tetris::TetriminoType::S, 13},
      {tetris::TetriminoType::Z, 14},
    };
  }
}

#endif
