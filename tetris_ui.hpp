#ifndef TETRIS_UI_HPP
#define TETRIS_UI_HPP

#include "tetris_game.hpp"
#include <ncurses.h>
#include <queue>

namespace tetris
{
  namespace ui
  {
    /* TODO */
    struct WindowInfo
    {
      short height, width;
      short v_offset, h_offset;
    };

    /* Convert a Point from playfield coordinates to draw window coordinates */
    game::Point playfield_point_to_draw_window_point(const game::Point& point);

    /* Create a new ncurses window from a WindowInfo object */
    WINDOW* create_window(const WindowInfo& window_info);

    /* Initialize the ncurses UI */
    void init_ui();

    /* Redraw the playfield and then the active tetrimino over it */
    void redraw_playfield(const game::Playfield& playfield, const game::Tetrimino& active_tetrimino);

    /* Redraw the player's current score and level */
    void redraw_score(long score, short level);

    /* Redraw a screen indicating the game is paused */
    void redraw_pause_screen();

    /* Window info constants */
    const WindowInfo PLAY_WINDOW_INFO{23, 22, -11, -11};
    const WindowInfo PREVIEW_WINDOW_INFO{23, 12, -11, 13};
    const WindowInfo SCORE_WINDOW_INFO{4, 20, -11, -33};

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

    /* Window pointer globals */
    extern WINDOW *play_window, *preview_window, *score_window;
  }
}

#endif
