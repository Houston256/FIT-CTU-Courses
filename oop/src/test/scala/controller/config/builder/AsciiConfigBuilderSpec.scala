package controller.config.builder

import controller.config.errors.*
import controller.config.mapper.*
import model.image.filter.*
import model.image.filter.grayToAscii.*
import model.image.io.loader.*
import model.image.io.writer.ascii.*
import org.scalatest.funspec.AnyFunSpec

class AsciiConfigBuilderSpec extends AnyFunSpec {
  describe("AsciiConfigBuilderSpec") {
    it("should build a valid PipelineConfig with all required components") {
      val builder = AsciiConfigBuilder()
      val updates = Seq(
        SetInput(FileInput("path/to/image.jpg")),
        SetTable(PredefinedTable("bourke-small")),
        AddFilter(InvertConfig),
        SetOutput(ConsoleOutput),
        SetOutput(FileOutput("file.jpg"))
      )

      val result = builder.build(updates)

      result match {
        case Right(config) =>
          assert(config.imageReader.isInstanceOf[ImageIOLoader])
          assert(config.toGray.isInstanceOf[RGBToGray])
          assert(config.finalFilter == Bourke)
          assert(config.imageWriters.exists(_.isInstanceOf[CLIWriter]))
          assert(config.imageWriters.exists(_.isInstanceOf[FileWriter]))
        case Left(err) =>
          fail(s"Expected valid PipelineConfig, but got error: $err")
      }
    }

    it("should set LogGrayToASCII") {
      val builder = AsciiConfigBuilder()
      val updates = Seq(
        SetInput(RandomInput(123)),
        SetTable(PredefinedTable("log")),
        SetOutput(ConsoleOutput)
      )
      val res = builder.build(updates)
      res match {
        case Left(err) => fail(s"build failed with error $err")
        case Right(pipeline) =>
          assert(pipeline.finalFilter == LogGrayToASCII)
      }

    }

    it("should fail if no input is specified") {
      val builder = AsciiConfigBuilder()
      val updates = Seq(
        SetTable(PredefinedTable("bourke-small")),
        AddFilter(InvertConfig),
        SetOutput(ConsoleOutput)
      )

      val result = builder.build(updates)

      assert(result == Left(UnspecifiedReaderError()))
    }

    it("should fail if no output is specified") {
      val builder = AsciiConfigBuilder()
      val updates = Seq(
        SetInput(FileInput("path/to/image.jpg")),
        SetTable(PredefinedTable("bourke-small")),
        AddFilter(InvertConfig)
      )

      val result = builder.build(updates)

      assert(result == Left(UnspecifiedWriterError()))
    }

    it("should allow multiple filters") {
      val builder = AsciiConfigBuilder()
      val updates = Seq(
        SetInput(RandomInput(12345)),
        SetTable(CustomTable(".-+*#")),
        AddFilter(InvertConfig),
        AddFilter(RotateConfig(90)),
        AddFilter(ScaleConfig(0.5)),
        SetOutput(ConsoleOutput)
      )

      val result = builder.build(updates)

      assert(result.isRight)
    }

    it("should fail with MultipleReaderError if multiple inputs are specified") {
      val builder = AsciiConfigBuilder()
      val updates = Seq(
        SetInput(FileInput("path/to/image1.jpg")),
        SetInput(FileInput("path/to/image2.jpg"))
      )

      val result = builder.build(updates)

      assert(result == Left(MultipleReaderError()))
    }

    it("should fail with MultipleTableError if multiple tables are specified") {
      val builder = AsciiConfigBuilder()
      val updates = Seq(
        SetInput(RandomInput(12345)),
        SetTable(PredefinedTable("bourke-small")),
        SetTable(CustomTable("hello-there df."))
      )

      val result = builder.build(updates)

      assert(result == Left(MultipleTableError()))
    }

    it("should handle unknown table names gracefully") {
      val builder = AsciiConfigBuilder()
      val updates = Seq(
        SetInput(RandomInput(12345)),
        SetTable(PredefinedTable("unknown-table"))
      )
      val result = builder.build(updates)
      assert(result == Left(UnknownTableName("unknown-table")))
    }
  }
}
