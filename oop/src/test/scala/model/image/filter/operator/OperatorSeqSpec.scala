package model.image.filter.operator
import fixtures.ImageFixtures
import model.core.matrix.DenseMatrix
import model.image.core.{GrayPixel, Image}
import model.image.filter.errors.InvalidRotationError
import org.scalatest.funspec.AnyFunSpec

class OperatorSeqSpec extends AnyFunSpec {
  describe("OperatorSeqSpec") {
    it("should be an identity if supplied with empty sequence") {
      val emptyPipe = OperatorSeq[GrayPixel](Seq())
      val res = emptyPipe(ImageFixtures.singlePixelImage)
      assert(res == Right(ImageFixtures.singlePixelImage))
    }
    it("should work with a single operator") {
      val pipe = OperatorSeq[GrayPixel](Seq(Scale(2)))
      val res = pipe(ImageFixtures.singlePixelImage)
      val expected = Image(
        DenseMatrix(
          Vector(
            Vector(GrayPixel(2))
          )))
      assert(res == Right(expected))
    }
    it("should work with a multiple operators") {
      val pipe = OperatorSeq[GrayPixel](Seq(Scale(2), Invert()))
      val res = pipe(ImageFixtures.singlePixelImage)
      val expected = Image(
        DenseMatrix(
          Vector(
            Vector(GrayPixel(253))
          )))
      assert(res == Right(expected))
    }
    it("should return first encountered error") {
      val pipe = OperatorSeq[GrayPixel](Seq(Rotate(1), Rotate(2)))
      val res = pipe(ImageFixtures.singlePixelImage)
      val expected = InvalidRotationError(1)
      assert(res == Left(expected))
    }
  }
}
