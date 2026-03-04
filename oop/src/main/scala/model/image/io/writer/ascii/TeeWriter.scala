package model.image.io.writer.ascii

import model.image.core.{ASCIIPixel, Image}
import model.image.io.errors.IOError
import model.image.io.writer.Writer

import java.io.File

class TeeWriter(val file: File) extends Writer[ASCIIPixel] {
  private val fileWriter = FileWriter(file)
  private val cliWriter = CLIWriter()
  override def write(img: Image[ASCIIPixel]): Either[IOError, Unit] = {
    val cli_res = cliWriter.write(img)
    if (cli_res.isLeft) {
      return cli_res
    }
    fileWriter.write(img)
  }
}
