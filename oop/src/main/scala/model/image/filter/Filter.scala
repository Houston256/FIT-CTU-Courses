package model.image.filter

import model.core.errors.AppError
import model.image.core.{GrayPixel, Image, Pixel, RGBPixel}

/**
 * Class which is responsible for the conversion of Image[A] to Image[B]
 * @tparam A type of input
 * @tparam B type of output
 */
trait Filter[A <: Pixel, B <: Pixel] {
  /**
   * Converts img
   * @param img input image
   * @return transformed image
   */
  def apply(img: Image[A]): Either[AppError, Image[B]]
}

