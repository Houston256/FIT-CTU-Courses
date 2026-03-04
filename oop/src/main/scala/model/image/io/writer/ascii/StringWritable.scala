package model.image.io.writer.ascii

import model.image.core.{ASCIIPixel, Image}

trait StringWritable {
  private def getRow(img: Image[ASCIIPixel], idx: Int): String =
    (0 until img.width)
      .map(
        img(idx, _)
          .getOrElse(
            throw new IllegalStateException("Unexpected out-of-bounds access"))
          .value)
      .mkString

  def asciiToString(img: Image[ASCIIPixel]): String = {
    val w = img.width
    val h = img.height
    val rows = (0 until h).map(getRow(img, _))
    rows.mkString("\n")
  }
}
