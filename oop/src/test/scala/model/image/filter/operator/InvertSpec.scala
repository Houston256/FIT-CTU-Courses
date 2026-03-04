package model.image.filter.operator

import model.core.matrix.DenseMatrix
import org.scalatest.funspec.AnyFunSpec
import model.image.core.{GrayPixel, Image}
import model.image.filter.operator.Invert

class InvertSpec extends AnyFunSpec {
  describe("Invert") {
    it("inverts pixel values") {
      val img = Image(DenseMatrix(Vector(
        Vector(GrayPixel(0), GrayPixel(128)),
        Vector(GrayPixel(255), GrayPixel(64))
      )))
      val inverted = Invert()(img).toOption.get
      assert(inverted(0,0).toOption.get.value == 255)
      assert(inverted(0,1).toOption.get.value == 127)
      assert(inverted(1,0).toOption.get.value == 0)
      assert(inverted(1,1).toOption.get.value == 191)
    }
  }
}
