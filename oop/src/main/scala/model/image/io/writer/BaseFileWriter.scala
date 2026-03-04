package model.image.io.writer

import model.image.core.Pixel
import model.image.io.file.FileSupport

trait BaseFileWriter[T <: Pixel] extends Writer[T] with FileSupport
