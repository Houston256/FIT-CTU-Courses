package controller.parser

/**
 * Represents argument from CLI
 * @example "--image img.jpg" -> ValueArgument("--image", "img.jpg")
 */
sealed trait RawArgument:
  /**
   * Name of the argument
   * @example "--image img.jpg" -> "--image"
   */
  def name: String

case class BoolArgument(name: String) extends RawArgument
case class ValueArgument(name: String, value: String) extends RawArgument