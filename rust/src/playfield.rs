use std::ops;

use crate::game::TetriminoType;
use crate::point::Point;

/// Grid in which the tetriminos fall.
///
/// Each cell in the grid stores a [TetriminoType] indicating what type of tetrimino has
/// been locked into that cell.
pub struct Playfield {
    grid: [[TetriminoType; Self::COLS]; Self::ROWS],
}

impl Playfield {
    const ROWS: usize = 40;
    const COLS: usize = 10;
}

impl Default for Playfield {
    fn default() -> Self {
        Playfield { grid: [[TetriminoType::NONE; Self::COLS]; Self::ROWS] }
    }
}

impl ops::Index<usize> for Playfield {
    type Output = [TetriminoType; Self::COLS];

    fn index(&self, index: usize) -> &Self::Output {
        &self.grid[index]
    }
}

impl ops::IndexMut<usize> for Playfield {
    fn index_mut(&mut self, index: usize) -> &mut Self::Output {
        &mut self.grid[index]
    }
}

impl ops::Index<Point> for Playfield {
    type Output = TetriminoType;

    fn index(&self, index: Point) -> &Self::Output {
        &self.grid[index.row][index.col]
    }
}

impl ops::IndexMut<Point> for Playfield {
    fn index_mut(&mut self, index: Point) -> &mut Self::Output {
        &mut self.grid[index.row][index.col]
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn playfield_index_usize() {
        let playfield = Playfield::default();
        assert_eq!(playfield[3][7], TetriminoType::NONE);
    }

    #[test]
    fn playfield_index_mut_usize() {
        let mut playfield = Playfield::default();
        playfield[3][7] = TetriminoType::O;
        assert_eq!(playfield[3][7], TetriminoType::O);
    }

    #[test]
    fn playfield_index_point() {
        let playfield = Playfield::default();
        let point = Point {row: 3, col: 7};
        assert_eq!(playfield[point], TetriminoType::NONE);
    }

    #[test]
    fn playfield_index_mut_point() {
        let mut playfield = Playfield::default();
        let point = Point {row: 3, col: 7};
        playfield[point] = TetriminoType::O;
        assert_eq!(playfield[point], TetriminoType::O);
    }
}
