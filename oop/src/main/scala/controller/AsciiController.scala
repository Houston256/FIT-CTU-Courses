package controller

import controller.config.PipelineConfig
import controller.config.builder.AsciiConfigBuilder
import controller.config.mapper.SimpleArgumentMapper
import controller.parser.AsciiParser
import model.core.errors.AppError

case class AsciiController() extends Controller {
  private val parser: AsciiParser = AsciiParser()
  private val mapper = SimpleArgumentMapper()
  private val builder = AsciiConfigBuilder()
  override def run(args: List[String]): Either[AppError, Unit] =
    for {
      rawArgs <- parser.parseEither(args)
      configUpdates <- mapper.processArguments(rawArgs)
      config <- builder.build(configUpdates)
      inputImg <- config.imageReader.load()
      gray <- config.toGray(inputImg)
      grayFiltered <- config.grayFilters(gray)
      ascii <- config.finalFilter(grayFiltered)
      writeResults = config.imageWriters.map(writer => writer.write(ascii))
      _ <- writeResults
        .collectFirst { case Left(error) => Left(error) }
        .getOrElse(Right(()))
    } yield Right(())
}
