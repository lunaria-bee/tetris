/// Enum to identify the type/shape of a tetrimino.
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

/// Enum to identify the facing of a tetrimino.
enum TetriminoFacing {
    NORTH,
    EAST,
    SOUTH,
    WEST,
}

/// Bitflags representing the results of collision tests.
bitflags! {
    struct CollisionResult: u8 {
        const NONE  = 0b0000;
        const WALL  = 0b0001;
        const FLOOR = 0b0010;
        const MINO  = 0b0100;
    }
}