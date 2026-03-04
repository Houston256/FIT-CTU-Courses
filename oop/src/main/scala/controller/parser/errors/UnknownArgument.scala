package controller.parser.errors

case class UnknownArgument(name: String) extends ParserError {
  override def message: String = s"Unknown argument with name: $name"
}
