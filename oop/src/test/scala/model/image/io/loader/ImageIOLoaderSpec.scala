package model.image.io.loader

import model.image.core.RGBPixel
import model.image.io.errors.FileLoadError
import model.image.io.loader.ImageIOLoader
import org.scalatest.funspec.AnyFunSpec
import utils.ResourceLoader
import org.scalatest.EitherValues.*

class ImageIOLoaderSpec extends AnyFunSpec {

  private def testImageLoading(
    filePath: String,
    expectedPixel: RGBPixel
  ): Unit = {
    val file = ResourceLoader.resourceFile(filePath)
    val loader = ImageIOLoader(file)
    val imgEither = loader.load()
    assert(imgEither.isRight)
    val img = imgEither.value
    assert(img.width == 1)
    assert(img.height == 1)
    assert(img(0, 0).value == expectedPixel)
  }

  private def testInvalidImage(filePath: String): Unit = {
    val file = ResourceLoader.resourceFile(filePath)
    val loader = ImageIOLoader(file)
    val imgEither = loader.load()
    assert(imgEither == Left(FileLoadError()))
  }

  describe("ImageIOLoader") {
    it("should load a valid PNG image") {
      testImageLoading("/images/1x1_RGB_1_2_3.png", RGBPixel(1, 2, 3))
    }

    it("should load a valid JPG image") {
      testImageLoading("/images/1x1_RGB_10_20_30.jpg", RGBPixel(10, 20, 30))
    }

    it("should load a valid GIF image") {
      testImageLoading("/images/1x1_RGB_50_60_70.gif", RGBPixel(50, 60, 70))
    }

    it("should fail on unsupported image format") {
      testInvalidImage("/images/1x1_RGB_255_255_255.bmp")
    }

    it("should fail on invalid image") {
      testInvalidImage("/images/invalid_image.png")
    }
  }
}
