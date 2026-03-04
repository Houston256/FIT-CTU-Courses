package model.image.io.errors

import model.core.errors.AppError

case class FileLoadError() extends AppError {
  override def message: String = "Problem with loading file."
}
