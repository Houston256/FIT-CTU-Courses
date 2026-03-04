package model.image.filter.operator

import fixtures.ImageFixtures
import model.core.matrix.DenseMatrix
import org.scalatest.funspec.AnyFunSpec
import model.image.core.{GrayPixel, Image}
import model.image.filter.errors.InvalidRotationError
import model.image.filter.operator.Rotate
import org.scalatest.EitherValues

class RotateSpec extends AnyFunSpec with EitherValues {
  describe("Rotate") {
    it("should return the original image when rotated by 0 turns") {
      val img = ImageFixtures.squareImage

      val result = Rotate(0)(img).value

      assert(
        result == img,
        "Image should remain unchanged when rotated by 0 turns")
    }

    it("should rotate the image correctly by 1 turn (90 degrees clockwise)") {

      val img = ImageFixtures.squareImage

      val expectedMatrix = DenseMatrix(
        Vector(
          Vector(GrayPixel(3), GrayPixel(1)),
          Vector(GrayPixel(4), GrayPixel(2))
        ))
      val expectedImg = Image(expectedMatrix)

      val result = Rotate(90)(img).value

      assert(
        result == expectedImg,
        "Image should be rotated 90 degrees clockwise")
    }

    it("should rotate the image correctly by 2 turns (180 degrees)") {
      val img = ImageFixtures.squareImage

      val expectedMatrix = DenseMatrix(
        Vector(
          Vector(GrayPixel(4), GrayPixel(3)),
          Vector(GrayPixel(2), GrayPixel(1))
        ))

      val expectedImg = Image(expectedMatrix)

      val result = Rotate(180)(img).value

      assert(result == expectedImg, "Image should be rotated 180 degrees")
    }

    it("should rotate the image correctly by 3 turns (270 degrees clockwise)") {
      val img = ImageFixtures.squareImage

      val expectedMatrix = DenseMatrix(
        Vector(
          Vector(GrayPixel(2), GrayPixel(4)),
          Vector(GrayPixel(1), GrayPixel(3))
        ))

      val expectedImg = Image(expectedMatrix)

      val result = Rotate(270)(img).value

      assert(
        result == expectedImg,
        "Image should be rotated 270 degrees clockwise")
    }

    it("should return InvalidRotationError for invalid number of turns") {
      val img = ImageFixtures.singlePixelImage

      val invalidTurns = Seq(-1, 91, 360, 1)

      invalidTurns.foreach { n =>
        val result = Rotate(n)(img)
        assert(result.isLeft, s"Expected Left for n_turns = $n")
        result.left.value match {
          case InvalidRotationError(turn) =>
            assert(
              turn == n,
              s"Expected InvalidRotationError with n_turns = $n, got $turn")
          case _ =>
            fail(
              s"Expected InvalidRotationError with n_turns = $n, got a different error")
        }
      }
    }

    it("handles empty images gracefully") {
      val emptyImg = ImageFixtures.emptyImage

      val result = Rotate(180)(emptyImg).value

      assert(result.height == 0)
      assert(result.width == 0)
      assert(
        result == emptyImg,
        "Empty image should remain unchanged after rotation")
    }
  }
}
