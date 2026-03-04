package model.image.filter.errors

import model.core.errors.AppError

case class EmptyTableError() extends AppError {
  override def message: String =
    "ASCII table is empty and therefore can't be used."
}
