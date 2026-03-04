package model.image.filter.operator

import org.scalatest.funspec.AnyFunSpec
import fixtures.ImageFixtures.*
import model.core.matrix.DenseMatrix
import model.image.core.{GrayPixel, Image}
import model.image.filter.operator.Scale
import org.scalatest.EitherValues.*


class ScaleSpec extends AnyFunSpec {
  describe("Scale operator") {
    it("should scale all pixel values by the factor") {
      val result = Scale(2)(squareImage)

      assert(result.isRight)

      val matrix = DenseMatrix(
        Vector(
          Vector(GrayPixel(2), GrayPixel(4)),
          Vector(GrayPixel(6), GrayPixel(8))
        ))

      assert(result.value == Image[GrayPixel](matrix))
    }

    it("should clamp pixel values to 255") {
      val scale = Scale(300)
      val result = scale(squareImage)

      assert(result.isRight)
      val matrix = DenseMatrix(
        Vector(
          Vector(GrayPixel(255), GrayPixel(255)),
          Vector(GrayPixel(255), GrayPixel(255))
        ))

      assert(result.value == Image[GrayPixel](matrix))
    }

    it("should clamp pixel values to the range 0") {
      val scale = Scale(-1)
      val result = scale(squareImage)

      assert(result.isRight)
      val matrix = DenseMatrix(
        Vector(
          Vector(GrayPixel(0), GrayPixel(0)),
          Vector(GrayPixel(0), GrayPixel(0))
        ))

      assert(result.value == Image[GrayPixel](matrix))
    }


    it("should handle empty images without errors") {
      val scale = Scale(2)
      val result = scale(emptyImage)

      assert(result.isRight)
      assert(result.value == emptyImage)
    }

    it("should scale a single-pixel image correctly") {
      val scale = Scale(3)
      val result = scale(singlePixelImage)

      val matrix = DenseMatrix(Vector(
        Vector(GrayPixel(3))
      ))
      assert(result.value == Image(matrix))
    }
  }
}
