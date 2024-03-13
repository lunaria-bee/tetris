use std::ops;

/// Two-dimensional point.
///
/// Used to represent points on the playfield and on the UI.
#[derive(Debug, PartialEq)]
pub struct Point {
    row: i16,
    col: i16,
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
            Point{row: -2, col: 7}
            + Point{row: 4, col: -17}
            + Point{row: 3, col: 3},
            Point{row: 5, col: -7}
        );
    }

    #[test]
    fn point_sub() {
        assert_eq!(
            Point{row: -2, col: 7}
            - Point{row: 4, col: -17}
            - Point{row: 3, col: 3},
            Point{row: -9, col: 21}
        );
    }

}
