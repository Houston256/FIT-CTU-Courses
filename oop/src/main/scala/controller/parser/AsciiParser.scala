package controller.parser

import controller.parser.errors.UnknownArgument

case class AsciiParser() extends Parser:
  def parse(args: List[String]): List[RawArgument] @throws[UnknownArgument] = args match
    case Nil => Nil
    case head :: tail if !head.startsWith("--") => throw UnknownArgument(head)
    // bool argument
    case head :: Nil => BoolArgument(head) :: Nil
    case head :: next :: tail if next.startsWith("--") => BoolArgument(head) :: parse(next :: tail)
    // value argument
    case head :: next :: tail if !next.startsWith("--") => ValueArgument(head, next) :: parse(tail)
    // else
    case head :: tail => throw UnknownArgument(head)
