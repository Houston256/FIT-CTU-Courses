package model.core.matrix.errors

import model.core.errors.AppError

case class BoundsError(n_rows: Int, n_cols: Int, row: Int, col: Int)
    extends AppError() {
  override def message: String =
    s"Can't access item at position ($row, $col), since the matrix shape is ($n_rows, $n_cols)."
}
