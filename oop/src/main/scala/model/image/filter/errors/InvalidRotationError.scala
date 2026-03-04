package model.image.filter.errors

case class InvalidRotationError(angle: Int) extends FilterError {
  def message: String =
    s"Invalid turn angle: $angle. Must be one of {0, 90, 180, 270}."
}
