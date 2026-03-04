package controller.parser.errors

import model.core.errors.AppError

trait ArgumentError extends AppError

case class IntConversionError(str: String) extends ArgumentError {
  override def message: String = s"Failed to convert $str to Int"
}

case class DoubleConversionError(str: String) extends ArgumentError {
  override def message: String = s"Failed to convert $str to Double"
}

case class UnknownArgumentError(argName: String) extends ArgumentError {
  override def message: String =
    s"Encountered unknown argument with name $argName"
}
