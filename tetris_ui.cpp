#include "tetris_ui.hpp"
#include "tetris.hpp"
#include <ncurses.h>
#include <array>
#include <chrono>
#include <fstream>
#include <thread>

Point playfield_point_to_draw_window_point(const Point& point)
{
  return Point(1+point.row-19, 1+point.col*2);
}

void draw_playbox()
{
  // Draw top border
  addwstr(L"┌");
  for (int i=0; i<20; i++)
    addwstr(L"─");
  addwstr(L"┐");

  // Draw side borders and rows
  for (int i=1; i<=21; i++)
  {
    mvaddwstr(i, 0, L"│");

    // Draw guide dots
    for (int j=0; j<10; j++)
      addstr(". ");

    addwstr(L"│");
  }

  // Draw bottom border
  mvaddwstr(22, 0, L"└");
  for(int i=0; i<20; i++)
    addwstr(L"─");
  addwstr(L"┘");
}

void redraw_playfield(const Playfield& playfield, const Tetrimino& active_tetrimino)
{
  // Draw playfield
  for (int i=19; i<40; i++)
  {
    for (int j=0; j<10; j++)
    {
      Point window_coords = playfield_point_to_draw_window_point(Point(i, j));
      move(window_coords.row, window_coords.col);
      // If mino is present, draw block
      if (playfield[i][j])
      {
        attron(COLOR_PAIR(playfield[i][j]));
        addstr("  ");
        attroff(COLOR_PAIR(playfield[i][j]));
      }
      // Otherwise, draw guide dot
      else
      {
        addstr(". ");
      }
    }
  }

  // Draw ghost at landing
  attron(COLOR_PAIR(GHOST_COLOR.at(active_tetrimino.type)));
  for (Point p : active_tetrimino.get_landing(playfield).points)
  {
    if (p.row >= 19)
    {
      Point window_coords = playfield_point_to_draw_window_point(p);
      mvaddwstr(window_coords.row, window_coords.col, L"[]");
    }
  }
  attroff(COLOR_PAIR(GHOST_COLOR.at(active_tetrimino.type)));

  // Draw active tetrimino
  attron(COLOR_PAIR(MINO_COLOR.at(active_tetrimino.type)));
  for (Point p : active_tetrimino.points)
  {
    if (p.row >= 19)
    {
      Point window_coords = playfield_point_to_draw_window_point(p);
      mvaddwstr(window_coords.row, window_coords.col, L"..");
    }
  }
  attroff(COLOR_PAIR(MINO_COLOR.at(active_tetrimino.type)));
}
