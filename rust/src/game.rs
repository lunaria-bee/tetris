use bitflags::bitflags;

/// Enum to identify the type/shape of a tetrimino.
#[derive(Clone, Copy, Debug, PartialEq)]
pub enum TetriminoType {
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
pub enum TetriminoFacing {
    NORTH,
    EAST,
    SOUTH,
    WEST,
}

bitflags! {
    /// Bitflags representing the results of collision tests.
    struct CollisionResult: u8 {
        const NONE  = 1;
        const WALL  = 1 << 1;
        const FLOOR = 1 << 2;
        const MINO  = 1 << 3;
    }
}
