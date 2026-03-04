package model.image.filter.operator

import model.core.errors.AppError
import model.image.core.{Image, Pixel}

case class OperatorSeq[A <: Pixel](private val operators: Seq[Operator[A]])
    extends Operator[A] {
  override def apply(img: Image[A]): Either[AppError, Image[A]] = {
    if (operators.isEmpty)
      return Right(img)
    operators.foldLeft[Either[AppError, Image[A]]](Right(img)) {
      case (Right(currentImage), operator) =>
        operator.apply(currentImage)
      case (Left(error), _) =>
        Left(error)
    }
  }
}
