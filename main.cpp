#include "tetris.hpp"
#include "tetris_ui.hpp"
#include <locale.h>
#include <ncurses.h>
#include <chrono>
#include <fstream>
#include <map>
#include <thread>

const std::map<int, Command> INPUT_MAP{
  {ERR, Command::DO_NOTHING},
  {'h', Command::SHIFT_LEFT},
  {'l', Command::SHIFT_RIGHT},
  {'j', Command::ROTATE_CCW},
  {'k', Command::ROTATE_CW},
  {'n', Command::SOFT_DROP},
  {' ', Command::HARD_DROP},
};

int main()
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
  init_pair(MINO_COLOR.at(TetriminoType::O), COLOR_WHITE, COLOR_WHITE);
  init_pair(MINO_COLOR.at(TetriminoType::I), COLOR_CYAN, COLOR_CYAN);
  init_pair(MINO_COLOR.at(TetriminoType::T), COLOR_MAGENTA, COLOR_MAGENTA);
  init_pair(MINO_COLOR.at(TetriminoType::L), COLOR_YELLOW, COLOR_YELLOW);
  init_pair(MINO_COLOR.at(TetriminoType::J), COLOR_BLUE, COLOR_BLUE);
  init_pair(MINO_COLOR.at(TetriminoType::S), COLOR_GREEN, COLOR_GREEN);
  init_pair(MINO_COLOR.at(TetriminoType::Z), COLOR_RED, COLOR_RED);
  init_pair(GHOST_COLOR.at(TetriminoType::O), COLOR_WHITE, 0);
  init_pair(GHOST_COLOR.at(TetriminoType::I), COLOR_CYAN, 0);
  init_pair(GHOST_COLOR.at(TetriminoType::T), COLOR_MAGENTA, 0);
  init_pair(GHOST_COLOR.at(TetriminoType::L), COLOR_YELLOW, 0);
  init_pair(GHOST_COLOR.at(TetriminoType::J), COLOR_BLUE, 0);
  init_pair(GHOST_COLOR.at(TetriminoType::S), COLOR_GREEN, 0);
  init_pair(GHOST_COLOR.at(TetriminoType::Z), COLOR_RED, 0);

  draw_playbox();
  refresh();
  getch();

  // Set up game
  Game game;
  game.active_tetrimino = game.bag.pop();
  std::chrono::steady_clock::time_point last_drop = std::chrono::steady_clock::now();
  std::ofstream logfile {"log.txt"};
  std::chrono::steady_clock::time_point tick_start = std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point tick_end = std::chrono::steady_clock::now();

  while (true)
  {
    tick_start = std::chrono::steady_clock::now();

    redraw_playfield(game.playfield, game.active_tetrimino);

    // Process input
    auto result = INPUT_MAP.find(getch());
    if (result != INPUT_MAP.end())
    {
      Command command = result->second;
      game.try_command(result->second);
      if (command == Command::SOFT_DROP)
      {
        logfile << std::chrono::duration_cast<std::chrono::milliseconds>(tick_start - last_drop).count()
                << " (soft drop)" << std::endl;
        last_drop = tick_start;
      }
    }

    // Process drop
    if (tick_start - last_drop >= game.get_drop_interval())
    {
      if (game.active_tetrimino.is_landed(game.playfield))
      {
        for (const Point& p : game.active_tetrimino.points)
          game.playfield[p] = MINO_COLOR.at(game.active_tetrimino.type);
        game.active_tetrimino = game.bag.pop();
      }
      else
      {
        game.active_tetrimino.fall(game.playfield);
      }
      logfile << std::chrono::duration_cast<std::chrono::milliseconds>(tick_start - last_drop).count()
              << std::endl;
      last_drop = tick_start;
    }

    // Delay until next tick
    tick_end = std::chrono::steady_clock::now();
    std::chrono::duration<float> work_time = tick_end - tick_start;
    if (work_time < tick_duration)
      std::this_thread::sleep_for(tick_duration - work_time);
  }

  // Close ncurses window and exit
  endwin();
  return 0;
}
