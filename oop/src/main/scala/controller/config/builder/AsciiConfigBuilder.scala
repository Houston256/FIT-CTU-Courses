package controller.config.builder

import controller.config.PipelineConfig
import controller.config.errors.*
import controller.config.mapper.*
import model.core.errors.AppError
import model.image.core.{ASCIIPixel, GrayPixel, RGBPixel}
import model.image.filter.grayToAscii.{
  Bourke,
  LinearGrayToASCII,
  LogGrayToASCII
}
import model.image.filter.operator.*
import model.image.filter.{Filter, RGBToGray}
import model.image.io.loader.{ImageIOLoader, Loader, RandomLoader}
import model.image.io.writer.Writer
import model.image.io.writer.ascii.{CLIWriter, FileWriter}

import java.io.File

case class AsciiConfigBuilder() extends ConfigBuilder[RGBPixel] {
  private var imageReader: Option[Loader[RGBPixel]] = None
  private var grayFilters: Seq[Operator[GrayPixel]] = Seq()
  private var finalFilter: Option[Filter[GrayPixel, ASCIIPixel]] = None
  private var imageWriters: Seq[Writer[ASCIIPixel]] = Seq()
  override def build(
    updates: Seq[ConfigUpdate]): Either[AppError, PipelineConfig[RGBPixel]] = {
    val results = updates.map(processUpdate)
    val error = results.collectFirst { case Left(err) => err }
    if (error.isDefined)
      return Left(error.get)
    if (imageWriters.isEmpty)
      return Left(UnspecifiedWriterError())
    for {
      reader <- imageReader.toRight(UnspecifiedReaderError())
      filter = finalFilter.getOrElse(Bourke)
    } yield
      PipelineConfig(
        imageReader = reader,
        toGray = RGBToGray(),
        grayFilters = OperatorSeq[GrayPixel](grayFilters),
        finalFilter = filter,
        imageWriters = imageWriters
      )
  }

  private def processUpdate(update: ConfigUpdate): Either[AppError, Unit] =
    update match {
      case SetInput(conf)  => setInput(conf)
      case SetTable(conf)  => setTable(conf)
      case AddFilter(conf) => addFilter(conf)
      case SetOutput(conf) => setOutput(conf)
    }

  private def setInput(inputConfig: InputConfig): Either[AppError, Unit] = {
    if (imageReader.isDefined)
      return Left(MultipleReaderError())
    inputConfig match {
      case FileInput(path) =>
        imageReader = Some(ImageIOLoader(File(path)))
      case RandomInput(seed) =>
        imageReader = Some(RandomLoader(seed))
    }
    Right(())
  }

  private def setTable(tableConfig: TableConfig): Either[AppError, Unit] = {
    if (finalFilter.isDefined)
      return Left(MultipleTableError())
    tableConfig match {
      case PredefinedTable(name) =>
        name match {
          case "bourke-small" =>
            finalFilter = Some(Bourke)
            Right(())
          case "log" =>
            finalFilter = Some(LogGrayToASCII)
            Right(())
          case name => Left(UnknownTableName(name))
        }

      case CustomTable(seq) =>
        finalFilter = Some(LinearGrayToASCII(seq))
        Right(())
    }
  }
  private def addFilter(filterConfig: FilterConfig): Either[AppError, Unit] =
    filterConfig match {
      case InvertConfig =>
        grayFilters = grayFilters :+ Invert()
        Right(())
      case RotateConfig(rotation) =>
        grayFilters = grayFilters :+ Rotate(rotation)
        Right(())
      case ScaleConfig(scale) =>
        grayFilters = grayFilters :+ Scale(scale)
        Right(())
    }
  private def setOutput(config: OutputConfig): Either[AppError, Unit] = {
    config match {
      case ConsoleOutput =>
        imageWriters = imageWriters :+ CLIWriter()
      case FileOutput(path) =>
        imageWriters = imageWriters :+ FileWriter(File(path))
    }
    Right(())
  }
}
