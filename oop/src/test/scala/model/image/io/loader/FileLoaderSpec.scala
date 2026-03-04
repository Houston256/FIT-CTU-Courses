package model.image.io.loader

import model.core.errors.AppError
import model.core.matrix.DenseMatrix
import model.image.core.{Image, Pixel, RGBPixel}
import model.image.io.loader.FileLoader

import java.io.File
import org.scalatest.funspec.AnyFunSpec

class FileLoaderSpec extends AnyFunSpec {
  // Minimal implementation of FileLoader for testing
  class LoaderMock(override val file: File) extends FileLoader[RGBPixel] {
    override def load(): Either[AppError, Image[RGBPixel]] =
      Right(Image(DenseMatrix(Vector.empty[Vector[RGBPixel]]))) // Not relevant for these tests
  }

  describe("FileLoaderSpec") {
    it("should detect invalid file") {
      val invalidFile = File("foo.bar")
      val loader = LoaderMock(invalidFile)
      assert(!loader.isFileValid)
    }

    it("should getFileExtension") {
      val file = File("abc..jpg..png")
      val loader = LoaderMock(file)
      assert(loader.getFileExtension.get == "png")
    }
  }
}
