package controller.config.mapper

import controller.parser.errors.{
  DoubleConversionError,
  IntConversionError,
  UnknownArgumentError
}
import controller.parser.{BoolArgument, ValueArgument}
import org.scalatest.funspec.AnyFunSpec

class SimpleArgumentMapperSpec extends AnyFunSpec {

  describe("SimpleArgumentMapper") {

    val argumentMapper = new SimpleArgumentMapper

    val testCases = List(
      // Valid cases
      BoolArgument("--invert") -> Right(AddFilter(InvertConfig)),
      ValueArgument("--rotate", "90") -> Right(AddFilter(RotateConfig(90))),
      ValueArgument("--scale", "0.5") -> Right(AddFilter(ScaleConfig(0.5))),
      ValueArgument("--image", "path/to/image.jpg") -> Right(
        SetInput(FileInput("path/to/image.jpg"))),
      BoolArgument("--output-console") -> Right(SetOutput(ConsoleOutput)),
      ValueArgument("--output-file", "path") -> Right(
        SetOutput(FileOutput("path"))),
      ValueArgument("--table", "tableName") -> Right(
        SetTable(PredefinedTable("tableName"))),
      ValueArgument("--custom-table", "abc") -> Right(
        SetTable(CustomTable("abc"))),
      // Error cases
      ValueArgument("--rotate", "not_a_number") -> Left(
        IntConversionError("not_a_number")),
      ValueArgument("--scale", "not_a_number") -> Left(
        DoubleConversionError("not_a_number")),
      BoolArgument("--unknown") -> Left(UnknownArgumentError("--unknown"))
    )

    testCases.foreach {
      case (input, expectedSingleOutput) =>
        it(s"should map $input to $expectedSingleOutput") {
          val result = argumentMapper.processArguments(List(input))
          assert(result == expectedSingleOutput.map(List(_)))
        }
    }
    it("should map 'BoolArgument(--image-random)' to SetInput(RandomInput(_)) with any value") {
      val input = List(BoolArgument("--image-random"))
      val result = argumentMapper.processArguments(input)

      result match {
        case Right(List(SetInput(RandomInput(_)))) => succeed
        case _                                     => fail(s"Unexpected result: $result")
      }
    }
  }
}
