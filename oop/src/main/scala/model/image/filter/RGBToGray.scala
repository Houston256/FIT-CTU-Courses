package model.image.filter

import model.core.matrix.errors.MapError
import model.image.core.{GrayPixel, Image, RGBPixel}

final case class RGBToGray() extends Filter[RGBPixel, GrayPixel] {
  override def apply(img: Image[RGBPixel]): Either[MapError, Image[GrayPixel]] =
    img.map(rgb_to_gray)

  private def rgb_to_gray(pixel: RGBPixel): GrayPixel =
    GrayPixel((0.3 * pixel.red + 0.59 * pixel.green + 0.11 * pixel.blue).toInt)
}
