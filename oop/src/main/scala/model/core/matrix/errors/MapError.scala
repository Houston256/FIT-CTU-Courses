package model.core.matrix.errors

case class MapError(exception_message: String) extends MatrixError {
  override def message: String =
    s"Mapping function threw an exception with message $exception_message."
}
