#ifndef TETRIS_HPP
#define TETRIS_HPP

#include <array>
#include <chrono>
#include <map>
#include <random>
#include <queue>

enum class TetriminoType
{
  O,
  I,
  T,
  L,
  J,
  S,
  Z,
};

enum class TetriminoFacing
{
  NORTH,
  EAST,
  SOUTH,
  WEST,
};

enum class Command
{
  DO_NOTHING,
  SHIFT_LEFT,
  SHIFT_RIGHT,
  ROTATE_CCW,
  ROTATE_CW,
  SOFT_DROP,
  HARD_DROP,
};

namespace CollisionResult
{
  const short NONE  = 0;
  const short WALL  = 1<<0;
  const short FLOOR = 1<<1;
  const short MINO  = 1<<2;
}

struct Point
{
  short row, col;

  Point()
    : row(0),
      col(0)
  {}

  Point(short row_init, short col_init)
    : row(row_init),
      col(col_init)
  {}

  Point operator+(const Point& right_op) const;
  Point& operator+=(const Point& right_op);
  Point operator-(const Point& right_op) const;
  Point& operator-=(const Point& right_op);
};

struct Playfield
{
  std::array<std::array<short, 10>, 40> grid{0};

  short get_column_base(short column) const;

  std::array<short, 10>& operator[](short index);
  const std::array<short, 10>& operator[](short index) const;
  short& operator[](const Point& point);
  short operator[](const Point& point) const;
};

struct Tetrimino
{
  TetriminoType type;
  Point pivot;
  std::array<Point, 4> points;
  TetriminoFacing facing;

  Tetrimino(TetriminoType type_init);

  bool translate(const Point& delta, const Playfield& playfield);
  bool fall(const Playfield& playfield);
  bool shift_left(const Playfield& playfield);
  bool shift_right(const Playfield& playfield);
  bool rotate_ccw(const Playfield& playfield);
  bool rotate_cw(const Playfield& playfield);
  bool is_landed(const Playfield& playfield) const;
  Tetrimino get_landing(const Playfield& playfield) const;
};

struct Bag
{
  std::queue<Tetrimino> tetrimino_queue;
  std::default_random_engine random_generator;

  Bag();

  Tetrimino pop();
  void extend_queue();
};

struct Game
{
  Playfield playfield;
  Bag bag;
  Tetrimino active_tetrimino{TetriminoType::I};
  short level = 1;

  bool try_command(Command command);
  std::chrono::duration<float> get_drop_interval();
};

short check_collision(const Point& point, const Playfield& playfield);

Point calculate_srs_offset(short point_index,
                           TetriminoType type,
                           TetriminoFacing facing_before,
                           TetriminoFacing facing_after);


/* Length of each game tick */
const std::chrono::duration<float> tick_duration(1.0/60.0);

//! 
// TODO fill in values
const std::map<TetriminoType, std::map<TetriminoFacing, std::array<Point, 4>>> SRS_OFFSET_VALUES{
  {TetriminoType::I, {
      {TetriminoFacing::NORTH, {Point(0, -1), Point(0, 2), Point(0, -1), Point(0, 2)}},
      {TetriminoFacing::EAST, {Point(0, 1), Point(0, 1), Point(-1, 1), Point(2, 1)}},
      {TetriminoFacing::SOUTH, {}},
      {TetriminoFacing::WEST, {}},
    }},
  {TetriminoType::T, {
      {TetriminoFacing::NORTH, {}},
      {TetriminoFacing::EAST,  {}},
      {TetriminoFacing::SOUTH, {}},
      {TetriminoFacing::WEST,  {}},
    }},
  {TetriminoType::L, {
      {TetriminoFacing::NORTH, {}},
      {TetriminoFacing::EAST,  {}},
      {TetriminoFacing::SOUTH, {}},
      {TetriminoFacing::WEST,  {}},
    }},
  {TetriminoType::J, {
      {TetriminoFacing::NORTH, {}},
      {TetriminoFacing::EAST,  {}},
      {TetriminoFacing::SOUTH, {}},
      {TetriminoFacing::WEST,  {}},
    }},
  {TetriminoType::S, {
      {TetriminoFacing::NORTH, {}},
      {TetriminoFacing::EAST,  {}},
      {TetriminoFacing::SOUTH, {}},
      {TetriminoFacing::WEST,  {}},
    }},
  {TetriminoType::Z, {
      {TetriminoFacing::NORTH, {}},
      {TetriminoFacing::EAST,  {}},
      {TetriminoFacing::SOUTH, {}},
      {TetriminoFacing::WEST,  {}},
    }},
};

#endif
