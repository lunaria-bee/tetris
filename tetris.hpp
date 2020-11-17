#ifndef TETRIS_HPP
#define TETRIS_HPP

#include <array>
#include <chrono>
#include <map>
#include <random>
#include <queue>

namespace tetris
{
  /* Enum to identify the type/shape of a tetrimino. */
  enum class TetriminoType
  {
    NONE,
    O,
    I,
    T,
    L,
    J,
    S,
    Z,
  };

  /* Enum to identify the facing of a tetrimino. */
  enum class TetriminoFacing
  {
    NORTH,
    EAST,
    SOUTH,
    WEST,
  };

  /* Enum to identify user game commands. */
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

  /* Bitmask to identify results of collision tests. */
  namespace CollisionResult
  {
    const short NONE  = 0;
    const short WALL  = 1<<0;
    const short FLOOR = 1<<1;
    const short MINO  = 1<<2;
  }

  /* Two-dimensional point on the playfield. */
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

  /* Grid in which the tetriminos fall.
   *
   * The state of a cell in the grid is represented by a short. A value of 0 indicates an
   * empty cell. Any other value indicates that a mino has been locked in the cell, with the
   * value corresponding to the type of tetrimino the locked mino was originally part of.
   */
  struct Playfield
  {
    std::array<std::array<TetriminoType, 10>, 40> grid{TetriminoType::NONE};

    short get_column_base(short column) const;

    std::array<TetriminoType, 10>& operator[](short index);
    const std::array<TetriminoType, 10>& operator[](short index) const;
    TetriminoType& operator[](const Point& point);
    TetriminoType operator[](const Point& point) const;
  };

  /* Tetris game piece. */
  struct Tetrimino
  {
    TetriminoType type;
    Point pivot;
    std::array<Point, 4> points;
    TetriminoFacing facing;

    Tetrimino(TetriminoType type_init);

    /* Translate a tetrimino by delta, if possible.
     *
     * Does not translate if a collision would result.
     *
     * delta: Offsets for translation.
     * playfield: Playfield on which translation will occur.
     * return: Whether translation was successful.
     */
    bool translate(const Point& delta, const Playfield& playfield);

    /* Rotate a tetrimino counter-clockwise, if possible.
     *
     * Does not rotate if a collision would result.
     *
     * playfield: Playfield on which translation will occur.
     * return: Whether rotation was successful.
     */
    bool rotate_ccw(const Playfield& playfield);

    /* Rotate a tetrimino clockwise, if possible.
     *
     * Does not rotate if a collision would result.
     *
     * playfield: Playfield on which translation will occur.
     * return: Whether rotation was successful.
     */
    bool rotate_cw(const Playfield& playfield);

    /* Drop tetrimino as far as possible.
     *
     * playfield: Playfield on which tranlsation will occur.
     * return: Whether translation was successful.
     */
    bool hard_drop(const Playfield& playfield);

    /* Check whether tetrimino has fallen as far as it can. */
    bool is_landed(const Playfield& playfield) const;

    /* Get the eventual landing point for a tetrimino, assuming it falls on its current
     * path.
     *
     * playfield: Playfield on which the tetrimino is falling.
     * return: Tetrimino with the state this one will have once it lands.
     */
    Tetrimino get_landing(const Playfield& playfield) const;
  };

  /* Semi-random generator for tetriminoes. */
  struct Bag
  {
    std::queue<Tetrimino> tetrimino_queue;
    std::default_random_engine random_generator;

    Bag();

    /* Remove a tetrimino from the end of the queue and return it.
     *
     * Will automatically extend the queue if no tetriminoes are available.
     */
    Tetrimino pop();

    /* Extend the queue with another set of seven tetriminoes. */
    void extend_queue();
  };

  /* Storage and control for game state. */
  struct Game
  {
    Playfield playfield;
    Bag bag;
    Tetrimino active_tetrimino{TetriminoType::I};
    short level = 1;
    short row_clears_to_next_level = level * 5; // TODO calculate row clears by variable goal system
    short max_level = 15;
    long score = 0; // typed for optimism
    // TODO score T-spins
    // TODO score hard drop
    // TODO score soft drop
    // TODO score back-to-back bonus

    /* Execute a command, if possible.
     *
     * command: Command to try.
     * return: Success or failure.
     * */
    bool try_command(Command command);

    /* TODO */
    void lock_active_tetrimino();

    /* Clear all full rows from the playfield. */
    void clear_rows();

    /* TODO */
    void draw_new_tetrimino();

    /* TODO */
    bool is_game_over();

    /* Get the drop interval based on the current level. */
    std::chrono::duration<float> get_drop_interval();
  };

  /* Check whether a point collides with any objects on the playfield. */
  short check_collision(const Point& point, const Playfield& playfield);
  short check_collision(const std::array<Point, 4>& points, const Playfield& playfield);

  /* TODO */
  bool process_srs(TetriminoType type,
                   const std::array<Point, 4>& points,
                   const Playfield playfield,
                   TetriminoFacing facing_before,
                   TetriminoFacing facing_after,
                   Point& offset);

  /* TODO */
  Point get_srs_offset_value(TetriminoType type, TetriminoFacing facing, short index);

  /* Determine how to translate a tetrimino following a rotation.
   *
   * Calculations are made in accordance with the super rotation system.
   */
  Point calculate_srs_offset(short point_index,
                             TetriminoType type,
                             TetriminoFacing facing_before,
                             TetriminoFacing facing_after);


  /* Length of each game tick. */
  const std::chrono::duration<float> TICK_DURATION(1.0/60.0);

  /* Maximum number of moves permitted in extended placement mode. */
  const short EXTENDED_PLACEMENT_MAX_MOVES = 15;

  /* Extended placement timer duration. */
  const std::chrono::duration<float> EXTENDED_PLACEMENT_MAX_TIME(0.5);

  /* Multiplied by level to increase score based on number of rows cleared at once */
  const std::map<short, short> row_clear_multipliers{
    {1, 100}, {2, 300}, {3, 500}, {4, 800}};

  const std::map<TetriminoFacing, std::array<Point, 4>> STANDARD_SRS_OFFSET_VALUES{
    {TetriminoFacing::NORTH, {Point(0, 0), Point(0, 0), Point(0, 0), Point(0, 0)}},
    {TetriminoFacing::EAST,  {Point(0, 1), Point(1, 1), Point(-2, 0), Point(-2, 1)}},
    {TetriminoFacing::SOUTH, {Point(0, 0), Point(0, 0), Point(0, 0), Point(0, 0)}},
    {TetriminoFacing::WEST,  {Point(0, -1), Point(1, -1), Point(-2, 0), Point(-2, -1)}},
  };

  const std::map<TetriminoFacing, std::array<Point, 4>> I_SRS_OFFSET_VALUES{
    {TetriminoFacing::NORTH, {Point(0, -1), Point(0, 2), Point(0, -1), Point(0, 2)}},
    {TetriminoFacing::EAST, {Point(0, 1), Point(0, 1), Point(-1, 1), Point(2, 1)}},
    {TetriminoFacing::SOUTH, {Point(0, 2), Point(0, -1), Point(1, 2), Point(1, -1)}},
    {TetriminoFacing::WEST, {Point(0, 0), Point(0, 0), Point(2, 0), Point(-1, 0)}},
  };
}

#endif
