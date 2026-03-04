package model.image.io.loader

import model.core.errors.AppError
import model.image.core.{Image, Pixel}

/**
 * Loads image of type T
 * @tparam T type of image
 */
trait Loader[T<:Pixel] {
  def load(): Either[AppError, Image[T]]
}

