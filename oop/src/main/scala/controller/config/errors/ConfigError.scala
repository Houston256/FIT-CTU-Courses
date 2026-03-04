package controller.config.errors

import model.core.errors.AppError

trait ConfigError extends AppError
case class UnknownUpdate() extends ConfigError {
  override def message: String = "Encountered an unknown ConfigUpdate"
}

case class UnknownTableName(name: String) extends ConfigError {
  override def message: String = s"Encountered an unknown table conversion with name: $name"
}

case class UnspecifiedWriterError() extends ConfigError {
  override def message: String = "Can't write image without a specified writer"
}
case class UnspecifiedReaderError() extends ConfigError {
  override def message: String = "Can't read image without a specified reader."
}

case class MultipleReaderError() extends ConfigError {
  override def message: String = "Can't read from multiple sources at once."
}

case class MultipleTableError() extends ConfigError {
  override def message: String = "Can't set multiple tables at once."
}