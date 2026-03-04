package model.image.filter.operator

import model.core.matrix.errors.MapError
import model.image.core.{GrayPixel, Image}

case class Scale(factor: Double) extends Operator[GrayPixel] {
  private def clamp(px: GrayPixel) =
    GrayPixel(math.max(0, math.min(255, (px.value * factor).toInt)))

  override def apply(
    img: Image[GrayPixel]): Either[MapError, Image[GrayPixel]] =
    img.map[GrayPixel](clamp)
}
