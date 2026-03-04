package model.image.io.loader

import model.core.errors.AppError
import model.core.matrix.DenseMatrix
import model.image.core.{Image, RGBPixel}

import scala.util.Random

case class RandomLoader(seed: Long) extends Loader[RGBPixel] {
  override def load(): Either[AppError, Image[RGBPixel]] = {
    val generator = Random(seed)
    val width = generator.nextInt(50) + 1
    val height = generator.nextInt(50) + 1
    val pixels = for {
      x <- 0 until width
      y <- 0 until height
    } yield {
      RGBPixel(generator.nextInt(256), generator.nextInt(256), generator.nextInt(256))
    }
    Right(Image(DenseMatrix.fromFlatSeq(pixels, width, height).get))
  }
}
