#ifndef TETRIS_UI_HPP
#define TETRIS_UI_HPP

#include "tetris.hpp"

/* Convert a Point from playfield coordinates to draw window coordinates */
Point playfield_to_draw_window(const Point& point);

/* Draw an outline around the playfield */
void draw_playbox();

/* Redraw the playfield and then the active tetrimino over it */
void redraw_playfield(const Playfield& playfield, const Tetrimino& active_tetrimino);

/* Map of (tetrimino type -> ncurses color code) for active and locked minoes */
const std::map<TetriminoType, short> MINO_COLOR{
  {TetriminoType::O, 1},
  {TetriminoType::I, 2},
  {TetriminoType::T, 3},
  {TetriminoType::L, 4},
  {TetriminoType::J, 5},
  {TetriminoType::S, 6},
  {TetriminoType::Z, 7},
};

/* Map of (tetrimino type -> ncurses color code) for ghost minoes */
const std::map<TetriminoType, short> GHOST_COLOR{
  {TetriminoType::O, 8},
  {TetriminoType::I, 9},
  {TetriminoType::T, 10},
  {TetriminoType::L, 11},
  {TetriminoType::J, 12},
  {TetriminoType::S, 13},
  {TetriminoType::Z, 14},
};

#endif
