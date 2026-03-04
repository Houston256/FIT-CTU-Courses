package model.image.filter.grayToAscii

import model.core.matrix.errors.MapError
import model.image.core.{ASCIIPixel, GrayPixel, Image}
import model.image.filter.errors.EmptyTableError
import model.image.filter.Filter

object Bourke extends Filter[GrayPixel, ASCIIPixel] {
  private val cvt = LinearGrayToASCII(
    "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ".reverse)

  override def apply(img: Image[GrayPixel])
    : Either[EmptyTableError | MapError, Image[ASCIIPixel]] = cvt.apply(img)
}
