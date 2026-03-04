package model.image.io.writer.ascii

import model.image.core.{ASCIIPixel, Image}
import model.image.io.errors.IOError
import model.image.io.writer.Writer

class CLIWriter extends Writer[ASCIIPixel] with StringWritable {
  override def write(img: Image[ASCIIPixel]): Either[IOError, Unit] = {
    val strToWrite = asciiToString(img)
    println(strToWrite)
    Right(())
  }
}
