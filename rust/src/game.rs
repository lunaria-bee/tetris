mod playfield;

use bitflags::bitflags;

/// Enum to identify the type/shape of a tetrimino.
#[derive(Clone, Copy, Debug, PartialEq)]
enum TetriminoType {
    NONE,
    O,
    I,
    T,
    L,
    J,
    S,
    Z,
}

impl Default for TetriminoType {
    fn default() -> Self { TetriminoType::NONE }
}

/// Enum to identify the facing of a tetrimino.
enum TetriminoFacing {
    NORTH,
    EAST,
    SOUTH,
    WEST,
}

bitflags! {
    /// Bitflags representing the results of collision tests.
    struct CollisionResult: u8 {
        const NONE  = 0b0000;
        const WALL  = 0b0001;
        const FLOOR = 0b0010;
        const MINO  = 0b0100;
    }
}
