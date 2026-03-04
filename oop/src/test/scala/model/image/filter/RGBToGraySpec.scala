package model.image.filter

import fixtures.ImageFixtures
import model.core.matrix.DenseMatrix
import model.image.core.{GrayPixel, Image, RGBPixel}
import org.scalatest.funspec.AnyFunSpec

class RGBToGraySpec extends AnyFunSpec {
  describe("RGBToGraySpec") {
    it("should convert correctly") {
      val cvt = RGBToGray()
      val res = cvt(ImageFixtures.singleRGB(RGBPixel(10, 20, 30)))
      val expPixel = GrayPixel((3 + 0.59 * 20 + 0.11 * 30).toInt)
      assert(res == Right(Image(DenseMatrix(Vector(Vector(expPixel))))))
    }
  }
}
