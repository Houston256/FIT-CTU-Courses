package model.image.io.writer.ascii

import model.image.core.{ASCIIPixel, Image}
import model.image.io.errors.{IOError, InvalidFile}
import model.image.io.writer.BaseFileWriter

import java.io.{File, PrintWriter}
import scala.util.Using

class FileWriter(override val file: File)
    extends BaseFileWriter[ASCIIPixel]
    with StringWritable {
  override def write(img: Image[ASCIIPixel]): Either[IOError, Unit] = {
    val strToWrite = asciiToString(img)
    val res = Using(new PrintWriter(file))(_.write(strToWrite)).toEither
    res match {
      case Right(_) => Right(())
      case Left(_)  => Left(InvalidFile(file.getName))
    }
  }
}
