package model.image.filter.operator

import model.image.core.Pixel
import model.image.filter.Filter

/**
 * Filter which maps over a single type
 * @tparam A type of input
 */
trait Operator[A <: Pixel] extends Filter[A, A]
