package model.image.filter.grayToAscii

import fixtures.ImageFixtures
import model.core.matrix.DenseMatrix
import model.core.matrix.errors.MapError
import model.image.core.{ASCIIPixel, GrayPixel, Image}
import model.image.filter.errors.EmptyTableError
import org.scalatest.funspec.AnyFunSpec

class LinearGrayToASCIISpec extends AnyFunSpec {
  describe("LinearGrayToASCII") {

    it("should convert a grayscale image to ASCII using a valid table") {
      val table = ".:lkfjkgs"
      val filter = LinearGrayToASCII(table)
      val grayImage = ImageFixtures.squareImage

      val result = filter(grayImage)

      val expected = Image(
        DenseMatrix(
          Vector(
            Vector(ASCIIPixel('.'), ASCIIPixel('.')),
            Vector(ASCIIPixel('.'), ASCIIPixel('.'))
          )
        ))

      assert(result == Right(expected))
    }

    it("should return EmptyTableError if the table is empty") {
      val table = ""
      val cvt = LinearGrayToASCII(table)

      val result = cvt(ImageFixtures.rectImage)

      assert(result == Left(EmptyTableError()))
    }
  }
}
