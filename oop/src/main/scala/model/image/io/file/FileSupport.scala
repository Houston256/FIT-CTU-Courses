package model.image.io.file

import java.io.File

/**
 * Trait which can be mixed-in to add file support
 */
trait FileSupport {
  val file: File

  final def isFileValid: Boolean =
    file.exists() && file.isFile

  final def getFileExtension: Option[String] = {
    val dotIndex = file.getName.lastIndexOf('.')
    if (dotIndex > 0 && dotIndex < file.getName.length - 1)
      Some(file.getName.substring(dotIndex + 1))
    else
      None
  }

}
