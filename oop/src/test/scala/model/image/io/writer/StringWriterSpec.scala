package model.image.io.writer

import model.core.matrix.DenseMatrix
import model.image.core.{ASCIIPixel, Image}
import model.image.io.writer.ascii.StringWritable
import org.scalatest.funspec.AnyFunSpec

class StringWriterSpec extends AnyFunSpec with StringWritable {
  describe("AsciiStringWriterSpec") {
    describe("AsciiStringWriter") {
      it("should convert a valid ASCII image to a string") {
        val matrix = DenseMatrix(
          Vector(
            Vector(ASCIIPixel('A'), ASCIIPixel('B'), ASCIIPixel('C')),
            Vector(ASCIIPixel('D'), ASCIIPixel('E'), ASCIIPixel('F'))
          ))
        val image = Image(matrix)

        val result = asciiToString(image)
        assert(result == "ABC\nDEF")
      }
    }
  }
}
