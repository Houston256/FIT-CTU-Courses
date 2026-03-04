package model.image.io.loader

import model.core.matrix.DenseMatrix
import model.image.core.{Image, RGBPixel}
import model.image.io.errors.FileLoadError

import java.awt.image.BufferedImage
import java.io.File
import javax.imageio.ImageIO
import scala.util.Try

class ImageIOLoader(override val file: File) extends FileLoader[RGBPixel] {
  private val SupportedFormats: Set[String] = Set("jpg", "png", "gif")

  private def bufferedToRGB(img: BufferedImage): Option[Image[RGBPixel]] = {
    val width = img.getWidth
    val height = img.getHeight
    val pixels = for {
      y <- 0 until height
      x <- 0 until width
    } yield {
      val rgb = img.getRGB(x, y)
      val red = (rgb >> 16) & 0xFF
      val green = (rgb >> 8) & 0xFF
      val blue = rgb & 0xFF
      RGBPixel(red, green, blue)
    }
    DenseMatrix.fromFlatSeq(pixels, width, height).map(Image.apply)
  }

  override def load(): Either[FileLoadError, Image[RGBPixel]] = {
    val ext = getFileExtension
    if (!isFileValid || ext.isEmpty || !SupportedFormats.contains(ext.get))
      return Left(FileLoadError())
    // get buffered image
    val buffOption = Try(ImageIO.read(file)).toOption
    if (buffOption.isEmpty) {
      return Left(FileLoadError())
    }
    // buffered to rgb
    val buffImage = buffOption.get
    if (buffImage == null) {
      return Left(FileLoadError())
    }
    bufferedToRGB(buffImage) match {
      case Some(img) => Right(img)
      case _ => Left(FileLoadError())
    }
  }
}
