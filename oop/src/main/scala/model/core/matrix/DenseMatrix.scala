package model.core.matrix

import model.core.matrix.errors.{BoundsError, MapError}

import scala.util.Try

object DenseMatrix {
  def fromFlatSeq[T](seq: Seq[T], width: Int, height: Int): Option[DenseMatrix[T]] = {
    if (seq.length != width * height || width < 0 || height < 0) None
    else {
      val rows = seq.grouped(width).toVector
      Some(DenseMatrix(rows.map(_.toVector)))
    }
  }
}

final case class DenseMatrix[A](private val data: Vector[Vector[A]])
    extends Matrix[A] {
  override def rows: Int = data.length

  override def cols: Int = if (data.isEmpty) 0 else data(0).length

  override def get(row: Int, col: Int): Either[BoundsError, A] = {
    check_bounds(row, col) || (return Left(
      BoundsError(rows, cols, row, col)))
    Right(data(row)(col))
  }

  override def update(
    row: Int,
    col: Int,
    value: A): Either[BoundsError, Matrix[A]] = {
    check_bounds(row, col) || (return Left(BoundsError(rows, cols, row, col)))
    Right(DenseMatrix[A](data.updated(row, data(row).updated(col, value))))
  }

  override def map[B](f: A => B): Either[MapError, Matrix[B]] =
    Try(data.map(_.map(f))).toEither.left
      .map(e => MapError(e.toString))
      .map(DenseMatrix[B])

  override def transpose: Matrix[A] = DenseMatrix[A](data.transpose)

  override def flipX: Matrix[A] = DenseMatrix[A](data.map(_.reverse))

  override def flipY: Matrix[A] = DenseMatrix[A](data.reverse)
}
