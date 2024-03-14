use std::ops;

/// Two-dimensional point.
///
/// Used to represent points on the playfield and on the UI.
#[derive(Clone, Copy, Debug, PartialEq)]
pub struct Point {
    pub row: usize,
    pub col: usize,
}

impl ops::Add for Point {
    type Output = Self;

    fn add(self, other: Self) -> Self {
        Self {
            row: self.row + other.row,
            col: self.col + other.col,
        }
    }
}

impl ops::Sub for Point {
    type Output = Self;

    fn sub(self, other: Self) -> Self {
        Self {
            row: self.row - other.row,
            col: self.col - other.col,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn point_add() {
        assert_eq!(
            Point{row: 2, col: 7}
            + Point{row: 4, col: 2}
            + Point{row: 3, col: 3},
            Point{row: 9, col: 12}
        );
    }

    #[test]
    fn point_sub() {
        assert_eq!(
            Point{row: 10, col: 13}
            - Point{row: 4, col: 2}
            - Point{row: 3, col: 5},
            Point{row: 3, col: 6}
        );
    }
}
