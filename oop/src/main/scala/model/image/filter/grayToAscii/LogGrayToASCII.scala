package model.image.filter.grayToAscii

import model.core.matrix.errors.MapError
import model.image.core.{ASCIIPixel, GrayPixel, Image}
import model.image.filter.Filter
import model.image.filter.errors.EmptyTableError
import model.image.filter.grayToAscii.Bourke

case object LogGrayToASCII extends Filter[GrayPixel, ASCIIPixel] {
  override def apply(img: Image[GrayPixel])
    : Either[EmptyTableError | MapError, Image[ASCIIPixel]] =
    img
      .map(pixel => GrayPixel(Math.log1p(pixel.value).toInt))
      .flatMap(Bourke.apply)
}
