package controller.config.mapper

import controller.parser.errors.{
  ArgumentError,
  DoubleConversionError,
  IntConversionError,
  UnknownArgumentError
}
import controller.parser.{BoolArgument, RawArgument, ValueArgument}

/**
 * Class responsible for conversion between RawArgument and config updates
 * @see ConfigUpdate
 */
trait ArgumentMapper {
  /**
   * Convert List of raw arguments into a list of updates, which should be applied to a configuration
   * @param arguments list of raw arguments
   * @return Either error or list of updates
   */
  def processArguments(
    arguments: List[RawArgument]): Either[ArgumentError, List[ConfigUpdate]]
}
