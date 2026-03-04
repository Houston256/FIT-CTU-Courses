package model.image.filter.operator

import model.image.core.{GrayPixel, Image}
import model.image.filter.errors.InvalidRotationError

case class Rotate(angle: Int) extends Operator[GrayPixel] {
  private def turn_img(img: Image[GrayPixel], n_turns: Int): Either[InvalidRotationError, Image[GrayPixel]] = {
    n_turns match
      case 0 => Right(img)
      case 90 => Right(img.transpose.flipX)
      case 180 => Right(img.flipX.flipY)
      case 270 => Right(img.transpose.flipY)
      case _ => Left(InvalidRotationError(n_turns))
  }
  override def apply(
    img: Image[GrayPixel]): Either[InvalidRotationError, Image[GrayPixel]] = turn_img(img, angle)
}

