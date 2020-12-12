#include "tetris_ui.hpp"
#include "tetris_game.hpp"
#include <ncurses.h>
#include <array>
#include <chrono>
#include <fstream>
#include <thread>


using namespace tetris;
using namespace tetris::ui;


game::Point tetris::ui::playfield_point_to_draw_window_point(const game::Point& point)
{
  return game::Point(1+point.row-19, 1+point.col*2);
}

void tetris::ui::init_ui()
{
  // Initialize ncurses
  initscr();
  curs_set(0);
  cbreak();
  noecho();
  nodelay(stdscr, true);
  keypad(stdscr, true);
  setlocale(LC_ALL, "");

  // Initialize colors, with tetromino types as keys
  start_color();
  use_default_colors();
  init_pair(MINO_COLOR.at(game::TetriminoType::O), COLOR_WHITE, COLOR_WHITE);
  init_pair(MINO_COLOR.at(game::TetriminoType::I), COLOR_CYAN, COLOR_CYAN);
  init_pair(MINO_COLOR.at(game::TetriminoType::T), COLOR_MAGENTA, COLOR_MAGENTA);
  init_pair(MINO_COLOR.at(game::TetriminoType::L), COLOR_YELLOW, COLOR_YELLOW);
  init_pair(MINO_COLOR.at(game::TetriminoType::J), COLOR_BLUE, COLOR_BLUE);
  init_pair(MINO_COLOR.at(game::TetriminoType::S), COLOR_GREEN, COLOR_GREEN);
  init_pair(MINO_COLOR.at(game::TetriminoType::Z), COLOR_RED, COLOR_RED);
  init_pair(GHOST_COLOR.at(game::TetriminoType::O), COLOR_WHITE, 0);
  init_pair(GHOST_COLOR.at(game::TetriminoType::I), COLOR_CYAN, 0);
  init_pair(GHOST_COLOR.at(game::TetriminoType::T), COLOR_MAGENTA, 0);
  init_pair(GHOST_COLOR.at(game::TetriminoType::L), COLOR_YELLOW, 0);
  init_pair(GHOST_COLOR.at(game::TetriminoType::J), COLOR_BLUE, 0);
  init_pair(GHOST_COLOR.at(game::TetriminoType::S), COLOR_GREEN, 0);
  init_pair(GHOST_COLOR.at(game::TetriminoType::Z), COLOR_RED, 0);

  draw_playbox();
  refresh();
  getch();
}

void tetris::ui::draw_playbox()
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

void tetris::ui::redraw_playfield(const game::Playfield& playfield, const game::Tetrimino& active_tetrimino)
{
  // Draw playfield
  for (int i=19; i<40; i++)
  {
    for (int j=0; j<10; j++)
    {
      game::Point window_coords = playfield_point_to_draw_window_point(game::Point(i, j));
      move(window_coords.row, window_coords.col);
      // If no mino is present, draw guide dot
      if (playfield[i][j] == game::TetriminoType::NONE)
      {
        addstr(". ");
      }
      // Otherwise, draw block
      else
      {
        attron(COLOR_PAIR(MINO_COLOR.at(playfield[i][j])));
        addstr("  ");
        attroff(COLOR_PAIR(MINO_COLOR.at(playfield[i][j])));
      }
    }
  }

  // Draw ghost at landing
  attron(COLOR_PAIR(GHOST_COLOR.at(active_tetrimino.type)));
  for (game::Point p : active_tetrimino.get_landing(playfield).points)
  {
    if (p.row >= 19)
    {
      game::Point window_coords = playfield_point_to_draw_window_point(p);
      mvaddwstr(window_coords.row, window_coords.col, L"[]");
    }
  }
  attroff(COLOR_PAIR(GHOST_COLOR.at(active_tetrimino.type)));

  // Draw active tetrimino
  attron(COLOR_PAIR(MINO_COLOR.at(active_tetrimino.type)));
  for (game::Point p : active_tetrimino.points)
  {
    if (p.row >= 19)
    {
      game::Point window_coords = playfield_point_to_draw_window_point(p);
      mvaddwstr(window_coords.row, window_coords.col, L"..");
    }
  }
  attroff(COLOR_PAIR(MINO_COLOR.at(active_tetrimino.type)));
}

void tetris::ui::redraw_score(long score, short level)
{
  mvprintw(1, 25, "Score: %ld", score);
  mvprintw(2, 25, "Level: %hd", level);
}

void tetris::ui::redraw_pause_screen()
{
  for (short i=19; i<40; i++)
  {
    game::Point window_coords = playfield_point_to_draw_window_point(game::Point(i, 0));
    mvaddwstr(window_coords.row, window_coords.col, L"                    ");
  }
  game::Point window_coords = playfield_point_to_draw_window_point(game::Point(29, 3));
  ++window_coords.col;
  mvaddwstr(window_coords.row, window_coords.col, L"PAUSED");
}
