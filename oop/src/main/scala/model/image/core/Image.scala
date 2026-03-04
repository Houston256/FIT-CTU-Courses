package model.image.core

import model.core.matrix.Matrix
import model.core.matrix.errors.{BoundsError, MapError}

/**
 * Immutable image of pixels with type A
 * @param mat underlying structure
 * @tparam A type of stored pixels
 */
case class Image[A <: Pixel](private val mat: Matrix[A]) {

  /**
   * @return Pixel value at (row, col) or out of bound error
   */
  def apply(row: Int, col: Int): Either[BoundsError, A] = mat.get(row, col)
  def updatePixel(row: Int, col: Int, value: A): Either[BoundsError, Image[A]] =
    mat.update(row, col, value).map(Image[A])
  def height: Int = mat.rows
  def width: Int = mat.cols
  def map[B <: Pixel](f: A => B): Either[MapError, Image[B]] =
    mat.map[B](f).map(Image[B])
  def flipX: Image[A] = Image[A](mat.flipX)
  def flipY: Image[A] = Image[A](mat.flipY)
  def transpose: Image[A] = Image[A](mat.T)
}
