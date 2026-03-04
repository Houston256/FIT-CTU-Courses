package model.image.io.loader
import model.image.core.Pixel
import model.image.io.file.FileSupport

trait FileLoader[T<:Pixel] extends Loader[T] with FileSupport