package controller.parser

import controller.parser.errors.ParserError

/**
 * Parses arguments from CLI
 */
trait Parser {
  /**
   * Converts strings to arguments which can further be processed 
   * @param args list of arguments from CLI
   * @return list of raw arguments
   * @throws ParserError if a problem is encountered
   */
  def parse(args: List[String]): List[RawArgument] @throws[ParserError]

  /**
   * Calls parse(args) and converts exception to Either
   * @param args list of arguments
   * @return either ParserError or list of raw arguments 
   * @see Parser.parse(...)
   */
  def parseEither(args: List[String]): Either[ParserError, List[RawArgument]] =
    try Right(parse(args))
    catch {
      case e: ParserError => Left(e)
    }
}
