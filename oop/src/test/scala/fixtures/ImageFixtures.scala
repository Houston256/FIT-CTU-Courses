package fixtures

import model.core.matrix.DenseMatrix
import model.image.core.{GrayPixel, Image, RGBPixel}

object ImageFixtures {
  val squareImage: Image[GrayPixel] = {
    val matrix = DenseMatrix(
      Vector(
        Vector(GrayPixel(1), GrayPixel(2)),
        Vector(GrayPixel(3), GrayPixel(4))
      ))
    Image(matrix)
  }

  def singleRGB(pixel: RGBPixel): Image[RGBPixel] = {
    val matrix = DenseMatrix(Vector(Vector(pixel)))
    Image(matrix)
  }

  val rectImage: Image[GrayPixel] = {
    val matrix = DenseMatrix(
      Vector(
        Vector(GrayPixel(1), GrayPixel(2)),
        Vector(GrayPixel(3), GrayPixel(4)),
        Vector(GrayPixel(5), GrayPixel(6))
      ))
    Image(matrix)
  }

  val singlePixelImage: Image[GrayPixel] = {
    val matrix = DenseMatrix(
      Vector(
        Vector(GrayPixel(1))
      ))
    Image(matrix)
  }

  val emptyImage: Image[GrayPixel] = {
    val matrix = DenseMatrix(
      Vector.empty[Vector[GrayPixel]]
    )
    Image(matrix)
  }
}
