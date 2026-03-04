package model.arg.parser

import controller.parser.{AsciiParser, BoolArgument, ValueArgument}
import controller.parser.errors.UnknownArgument
import org.scalatest.funspec.AnyFunSpec

class AsciiParserSpec extends AnyFunSpec {

  describe("AsciiParser") {
    val parser = AsciiParser()
    val inputs = List(
      List(
        "--image",
        "../images/test-image.jpg",
        "--rotate",
        "+90",
        "--scale",
        "0.25",
        "--invert",
        "--output-console"),
      List(
        "--image",
        "test-image.jpg",
        "--output-file",
        "../outputs/output.txt"),
      List(
        "--image",
        "test-image.jpg",
        "--rotate",
        "+90",
        "--invert",
        "--output-file",
        "../outputs/output.txt",
        "--output-console",
        "--table",
        "bourke-small"),
      List("--random-image")
    )
    val outputs = List(
      List(
        ValueArgument("--image", "../images/test-image.jpg"),
        ValueArgument("--rotate", "+90"),
        ValueArgument("--scale", "0.25"),
        BoolArgument("--invert"),
        BoolArgument("--output-console")
      ),
      List(
        ValueArgument("--image", "test-image.jpg"),
        ValueArgument("--output-file", "../outputs/output.txt")),
      List(
        ValueArgument("--image", "test-image.jpg"),
        ValueArgument("--rotate", "+90"),
        BoolArgument("--invert"),
        ValueArgument("--output-file", "../outputs/output.txt"),
        BoolArgument("--output-console"),
        ValueArgument("--table", "bourke-small")
      ),
      List(BoolArgument("--random-image"))
    )
    it("should parse example commands") {
      inputs.zip(outputs).foreach {
        case (in, out) =>
          val res = parser.parse(in)
          val resEither = parser.parseEither(in)
          assert(resEither.isRight)
          assert(resEither == Right(out))
          assert(res == out)
      }
    }
    it("should fail on invalid commads") {
      val in = List("--image", "first", "second")
      assertThrows[UnknownArgument](parser.parse(in))
      assert(parser.parseEither(in) == Left(UnknownArgument("second")))
    }
  }
}
