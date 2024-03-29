#include "tetris_ui.hpp"
#include "tetris_game.hpp"
#include <ncurses.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>


using namespace tetris;
using namespace tetris::ui;


WINDOW *tetris::ui::play_window, *tetris::ui::preview_window, *tetris::ui::score_window;


game::Point tetris::ui::playfield_point_to_draw_window_point(const game::Point& point)
{
  return game::Point(1+point.row-19, 1+point.col*2);
}

WINDOW* tetris::ui::create_window(const WindowInfo& window_info)
{
  return newwin(window_info.height, window_info.width,
                LINES / 2 + window_info.v_offset,
                COLS / 2 + window_info.h_offset);
}

void tetris::ui::init_ui(short preview_size)
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

  // Calculate actual preview window info from base
  WindowInfo preview_window_info(PREVIEW_WINDOW_INFO);
  preview_window_info.height = 3 * preview_size + 3;

  // Create windows
  play_window = create_window(PLAY_WINDOW_INFO);
  preview_window = create_window(preview_window_info);
  score_window = create_window(SCORE_WINDOW_INFO);
  box(play_window, 0, 0);
  box(preview_window, 0, 0);
  box(score_window, 0, 0);
  refresh();
  wrefresh(play_window);
  wrefresh(preview_window);
  wrefresh(score_window);
}

void tetris::ui::redraw_playfield(const game::Playfield& playfield, const game::Tetrimino& active_tetrimino)
{
  // Draw playfield
  for (int i=19; i<40; i++)
  {
    for (int j=0; j<10; j++)
    {
      game::Point window_coords = playfield_point_to_draw_window_point(game::Point(i, j));
      wmove(play_window, window_coords.row, window_coords.col);
      // If no mino is present, draw guide dot
      if (playfield[i][j] == game::TetriminoType::NONE)
      {
        waddstr(play_window, ". ");
      }
      // Otherwise, draw block
      else
      {
        wattron(play_window, COLOR_PAIR(MINO_COLOR.at(playfield[i][j])));
        waddstr(play_window, "  ");
        wattroff(play_window, COLOR_PAIR(MINO_COLOR.at(playfield[i][j])));
      }
    }
  }

  // Draw ghost at landing
  wattron(play_window, COLOR_PAIR(GHOST_COLOR.at(active_tetrimino.type)));
  for (game::Point p : active_tetrimino.get_landing(playfield).points)
  {
    if (p.row >= 19)
    {
      game::Point window_coords = playfield_point_to_draw_window_point(p);
      mvwaddwstr(play_window, window_coords.row, window_coords.col, L"[]");
    }
  }
  wattroff(play_window, COLOR_PAIR(GHOST_COLOR.at(active_tetrimino.type)));

  // Draw active tetrimino
  wattron(play_window, COLOR_PAIR(MINO_COLOR.at(active_tetrimino.type)));
  for (game::Point p : active_tetrimino.points)
  {
    if (p.row >= 19)
    {
      game::Point window_coords = playfield_point_to_draw_window_point(p);
      mvwaddwstr(play_window, window_coords.row, window_coords.col, L"..");
    }
  }
  wattroff(play_window, COLOR_PAIR(MINO_COLOR.at(active_tetrimino.type)));

  wrefresh(play_window);
}

void tetris::ui::redraw_score(long score, short rows, short level)
{
  mvwprintw(score_window, 1, 2, "Score: %-8ld", score);
  mvwprintw(score_window, 2, 2, "Rows:  %-8hd", rows);
  mvwprintw(score_window, 3, 2, "Level: %-8hd", level);
  wrefresh(score_window);
}

void tetris::ui::redraw_preview(const std::deque<game::Tetrimino>& tetrimino_queue,
                                short preview_size)
{
  wclear(preview_window);
  box(preview_window, 0, 0);

  game::Point draw_base{2, -4};

  for (int i=0; i<preview_size; i++)
  {
    game::Tetrimino tetrimino = tetrimino_queue[i];

    wattron(preview_window, COLOR_PAIR(MINO_COLOR.at(tetrimino.type)));
    for (game::Point tetrimino_point : tetrimino.points)
    {
      game::Point draw_point = draw_base + playfield_point_to_draw_window_point(tetrimino_point);
      if (tetrimino.type == game::TetriminoType::I)
        draw_point.row -= 1;

      mvwaddwstr(preview_window, draw_point.row, draw_point.col, L"..");
    }
    wattroff(preview_window, COLOR_PAIR(MINO_COLOR.at(tetrimino.type)));

    if (tetrimino.type == game::TetriminoType::I)
      draw_base += game::Point(2, 0);
    else
      draw_base += game::Point(3, 0);
  }

  wrefresh(preview_window);
}

void tetris::ui::redraw_window_text(WINDOW* window,
                                    const WindowInfo& window_info,
                                    const std::wstring& text,
                                    const game::Point& offset)
{
  int newline_count = std::count(text.begin(), text.end(), '\n');

  game::Point draw_point;
  draw_point.row = (window_info.height / 2) - (newline_count / 2) + offset.row;

  std::wstringstream stream(text);
  std::wstring line;
  while (std::getline(stream, line, L'\n'))
  {
    draw_point.col = (window_info.width / 2) - (line.length() / 2) + offset.col;
    mvwaddwstr(window, draw_point.row, draw_point.col, line.c_str());
    ++draw_point.row;
  }
}

void tetris::ui::redraw_pause_screen()
{
  for (short i=19; i<40; i++)
  {
    game::Point window_coords = playfield_point_to_draw_window_point(game::Point(i, 0));
    mvwaddwstr(play_window, window_coords.row, window_coords.col, L"                    ");
  }
  redraw_window_text(play_window, PLAY_WINDOW_INFO, L"PAUSED");

  wrefresh(play_window);
}

void tetris::ui::redraw_game_over_screen()
{
  for (short i=19; i<40; i++)
  {
    game::Point window_coords = playfield_point_to_draw_window_point(game::Point(i, 0));
    mvwaddwstr(play_window, window_coords.row, window_coords.col, L"                    ");
  }

  std::wstringstream game_over_text;
  game_over_text << L"GAME OVER" << std::endl
                 << std::endl
                 << L"[r] Retry" << std::endl
                 << L"[q] Quit" << std::endl;
  redraw_window_text(play_window, PLAY_WINDOW_INFO, game_over_text.str());

  wrefresh(play_window);
}
