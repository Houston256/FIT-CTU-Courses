package model.image.io.writer

import model.image.core.{ASCIIPixel, Image, Pixel}
import model.image.io.errors.IOError

/**
 * Writes image of type T
 * @tparam T type of image
 */
trait Writer[T <: Pixel] {
  def write(img: Image[T]): Either[IOError, Unit]
}
