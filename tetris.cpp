#include "tetris.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <random>


/* Point Class Methods */

Point Point::operator+(const Point& right_op) const
{
  return Point(this->row + right_op.row, this->col + right_op.col);
}

Point& Point::operator+=(const Point& right_op)
{
  this->row += right_op.row;
  this->col += right_op.col;
  return *this;
}

Point Point::operator-(const Point& right_op) const
{
  return Point(this->row - right_op.row, this->col - right_op.col);
}

Point& Point::operator-=(const Point& right_op)
{
  this->row -= right_op.row;
  this->col -= right_op.col;
  return *this;
}


/* Playfield Class Methods */

std::array<short, 10>& Playfield::operator[](short index)
{
  return grid[index];
}

const std::array<short, 10>& Playfield::operator[](short index) const
{
  return grid[index];
}

short& Playfield::operator[](const Point& point)
{
  return grid[point.row][point.col];
}

short Playfield::operator[](const Point& point) const
{
  return grid[point.row][point.col];
}


/* Tetrimino Class Methods */

Tetrimino::Tetrimino(TetriminoType type_init)
  : type(type_init),
    facing(TetriminoFacing::NORTH)
{
  switch(type)
  {
    case TetriminoType::O:
      points = {
        Point(18, 4),
        Point(18, 5),
        Point(19, 4),
        Point(19, 5),
      };
      pivot = Point(19, 4);
      break;

    case TetriminoType::I:
      points = {
        Point(19, 3),
        Point(19, 4),
        Point(19, 5),
        Point(19, 6),
      };
      pivot = Point(19, 4);
      break;

    case TetriminoType::T:
      points = {
        Point(18, 4),
        Point(19, 3),
        Point(19, 4),
        Point(19, 5),
      };
      pivot = Point(19, 4);
      break;

    case TetriminoType::L:
      points = {
        Point(18, 5),
        Point(19, 3),
        Point(19, 4),
        Point(19, 5),
      };
      pivot = Point(19, 4);
      break;

    case TetriminoType::J:
      points = {
        Point(18, 3),
        Point(19, 3),
        Point(19, 4),
        Point(19, 5),
      };
      pivot = Point(19, 4);
      break;

    case TetriminoType::S:
      points = {
        Point(18, 4),
        Point(18, 5),
        Point(19, 3),
        Point(19, 4),
      };
      pivot = Point(18, 4);
      break;

    case TetriminoType::Z:
      points = {
        Point(18, 3),
        Point(18, 4),
        Point(19, 4),
        Point(19, 5),
      };
      pivot = Point(18, 4);
      break;
  }
}

bool Tetrimino::translate(const Point& delta, const Playfield& playfield)
{
  Point new_pivot = pivot + delta;
  std::array<Point, 4> new_points = points;
  for (Point& p : new_points)
  {
    p += delta;
    if (check_collision(p, playfield) != CollisionResult::NONE)
      return false;
  }

  pivot = new_pivot;
  points = new_points;
  return true;
}

bool Tetrimino::rotate_ccw(const Playfield& playfield)
{
  if (type == TetriminoType::O)
    return true;

  std::array<Point, 4> new_points = points;
  for (Point& p : new_points)
  {
    p -= pivot;
    p = Point(-p.col, p.row);
    p += pivot;

    if (check_collision(p, playfield) != CollisionResult::NONE)
      return false;
  }

  points = new_points;
  return true;
}

bool Tetrimino::rotate_cw(const Playfield& playfield)
{
  if (type == TetriminoType::O)
    return true;

  std::array<Point, 4> new_points = points;
  for (Point& p : new_points)
  {
    p -= pivot;
    p = Point(p.col, -p.row);
    p += pivot;

    if (check_collision(p, playfield) != CollisionResult::NONE)
      return false;
  }

  points = new_points;
  return true;
}

bool Tetrimino::hard_drop(const Playfield& playfield)
{
  short fall_distance = get_landing(playfield).pivot.row - pivot.row;
  return translate(Point(fall_distance, 0), playfield);
}

bool Tetrimino::is_landed(const Playfield& playfield) const
{
  for (const Point& p : points)
  {
    if (check_collision(p + Point(1, 0), playfield))
      return true;
  }

  return false;
}

Tetrimino Tetrimino::get_landing(const Playfield& playfield) const
{
  short distance_to_landing = 41;

  for (short row=0; row<40; row++)
  {
    for (const Point& p : points)
    {
      if (playfield[row][p.col] && (row - p.row - 1 < distance_to_landing) && (row - p.row - 1 >= 0))
        distance_to_landing = row - p.row - 1;
    }
  }

  for (const Point& p : points)
  {
    if (39 - p.row < distance_to_landing)
      distance_to_landing = 39 - p.row;
  }

  Tetrimino landing = *this;
  landing.translate(Point(distance_to_landing, 0), playfield);
  return landing;
}


/* Bag Class Methods */

Bag::Bag()
{
  std::random_device rd;
  random_generator = std::default_random_engine(rd());
  extend_queue();
}

Tetrimino Bag::pop()
{
  Tetrimino next = tetrimino_queue.front();
  tetrimino_queue.pop();
  if (tetrimino_queue.size() < 7)
    extend_queue();

  return next;
}

void Bag::extend_queue()
{
  std::array<Tetrimino, 7> tetriminos {
    Tetrimino(TetriminoType::O),
    Tetrimino(TetriminoType::I),
    Tetrimino(TetriminoType::T),
    Tetrimino(TetriminoType::L),
    Tetrimino(TetriminoType::J),
    Tetrimino(TetriminoType::S),
    Tetrimino(TetriminoType::Z),
  };

  std::shuffle(tetriminos.begin(), tetriminos.end(), random_generator);

  for (Tetrimino t : tetriminos)
    tetrimino_queue.push(t);
}


/* Game Class Methods */

bool Game::try_command(Command command)
{
  switch(command)
  {
    case Command::DO_NOTHING:
      break;

    case Command::SHIFT_LEFT:
      active_tetrimino.translate(Point(0, -1), playfield);
      break;

    case Command::SHIFT_RIGHT:
      active_tetrimino.shift_right(Point(0, 1), playfield);
      break;

    case Command::ROTATE_CCW:
      active_tetrimino.rotate_ccw(playfield);
      break;

    case Command::ROTATE_CW:
      active_tetrimino.rotate_cw(playfield);
      break;

    case Command::SOFT_DROP:
      active_tetrimino.translate(Point(1, 0), playfield);
      break;

    case Command::HARD_DROP:
      active_tetrimino.hard_drop(playfield);
      break;
  }

  return true;
}

void Game::clear_rows()
{
  for (int row=39; row>0; row--)
  {
    // Check if row is full
    bool row_full(true);
    for (short col=0; col<10; col++)
    {
      if (!playfield[row][col])
      {
        row_full = false;
        break;
      }
    }

    // If row is full, clear it and lower upper rows
    if (row_full)
    {
      for (int rowc=row; rowc>0; rowc--)
      {
        for (int colc=0; colc<10; colc++)
          playfield[rowc][colc] = playfield[rowc-1][colc];
      }
      row++;
    }
  }
}

std::chrono::duration<float> Game::get_drop_interval()
{
  return std::chrono::duration<float>(pow(0.8 - ((level-1) * 0.0007), level-1));
}


/* Free Functions */

short check_collision(const Point& point, const Playfield& playfield)
{
  short result = CollisionResult::NONE;

  if (point.row > 39)
    result |= CollisionResult::FLOOR;

  if (point.col < 0 || point.col > 9)
    result |= CollisionResult::WALL;

  if (playfield[point.row][point.col])
    result |= CollisionResult::MINO;

  return result;
}

Point calculate_srs_offset(short point_index,
                           TetriminoType type,
                           TetriminoFacing facing_before,
                           TetriminoFacing facing_after)
{
  return (SRS_OFFSET_VALUES.at(type).at(facing_after)[point_index]
          - SRS_OFFSET_VALUES.at(type).at(facing_before)[point_index]);
}
