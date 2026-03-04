package controller.config.mapper

import controller.config.errors.*
import controller.config.mapper.ArgumentMapper
import controller.parser.*
import controller.parser.errors.*

class SimpleArgumentMapper extends ArgumentMapper {
  private def mapArgument(
    argument: RawArgument): Either[ArgumentError, ConfigUpdate] =
    argument match {
      case BoolArgument("--invert") =>
        Right(AddFilter(InvertConfig))

      case ValueArgument("--rotate", value) =>
        try Right(AddFilter(RotateConfig(value.toInt)))
        catch {
          case _: NumberFormatException =>
            Left(IntConversionError(value))
        }

      case ValueArgument("--scale", value) =>
        try Right(AddFilter(ScaleConfig(value.toDouble)))
        catch {
          case _: NumberFormatException =>
            Left(DoubleConversionError(value))
        }

      case ValueArgument("--image", path) =>
        Right(SetInput(FileInput(path)))

      case BoolArgument("--image-random") =>
        // adding an optional ValueArgument with seed should still be easy, but it isn't in the assignment...
        Right(SetInput(RandomInput(System.nanoTime())))

      case BoolArgument("--output-console") =>
        Right(SetOutput(ConsoleOutput))

      case ValueArgument("--output-file", path) =>
        Right(SetOutput(FileOutput(path)))

      case ValueArgument("--table", name) =>
        Right(SetTable(PredefinedTable(name)))

      case ValueArgument("--custom-table", sequence) =>
        Right(SetTable(CustomTable(sequence)))

      case unknown => Left(UnknownArgumentError(unknown.name))
    }

  override def processArguments(
    arguments: List[RawArgument]): Either[ArgumentError, List[ConfigUpdate]] = {
    val results = arguments.map(mapArgument)

    val errors = results.collect { case Left(err) => err }

    if (errors.nonEmpty)
      Left(errors.head)
    else
      Right(results.collect { case Right(update) => update })
  }
}
