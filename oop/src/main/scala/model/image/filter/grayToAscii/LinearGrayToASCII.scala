package model.image.filter.grayToAscii

import model.core.matrix.errors.MapError
import model.image.core.{ASCIIPixel, GrayPixel, Image}
import model.image.filter.Filter
import model.image.filter.errors.EmptyTableError

/**
 * General fulter for ascii conversions
 * @param table sequence of characters
 */
final case class LinearGrayToASCII(private val table: Seq[Char])
    extends Filter[GrayPixel, ASCIIPixel] {
  override def apply(img: Image[GrayPixel])
    : Either[EmptyTableError | MapError, Image[ASCIIPixel]] = {
    table.nonEmpty || (return Left(EmptyTableError()))
    img.map(convertGrayToASCII)
  }

  private def convertGrayToASCII(grayPixel: GrayPixel): ASCIIPixel = {
    val index = (grayPixel.value.toDouble / 255 * (table.size - 1)).toInt
    ASCIIPixel(table(index))
  }
}


