package model.image.filter.operator

import model.core.matrix.errors.MapError
import model.image.core.{GrayPixel, Image}

import java.lang.Math.{max, min}

case class Invert() extends Operator[GrayPixel] {
  private def invert_pixel(pixel: GrayPixel, color_depth: Int): GrayPixel =
    GrayPixel(min(max(0, color_depth - pixel.value), color_depth))
  override def apply(
    img: Image[GrayPixel]): Either[MapError, Image[GrayPixel]] =
    img.map(invert_pixel(_, 255))
}
