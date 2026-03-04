package model.core.matrix

import model.core.matrix.errors.{BoundsError, MapError}

/**
 * Stores a matrix of values of any type
 * @tparam A type of stored value
 */
trait Matrix[A]() {
  protected def check_bounds(row: Int, col: Int): Boolean =
    row >= 0 && row < rows && col >= 0 && col < cols

  def rows: Int
  def cols: Int
  def get(row: Int, col: Int): Either[BoundsError, A]
  def update(row: Int, col: Int, value: A): Either[BoundsError, Matrix[A]]
  def map[B](f: A => B): Either[MapError, Matrix[B]]
  def transpose: Matrix[A]
  def T: Matrix[A] = transpose
  def flipX: Matrix[A]
  def flipY: Matrix[A]
}
