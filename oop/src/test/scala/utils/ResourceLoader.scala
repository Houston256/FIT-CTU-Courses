package utils

import java.io.File
import java.net.URL

object ResourceLoader {
  def resourceFile(path: String): File = {
    val resourceUrl: URL = getClass.getResource(path)
    assert(resourceUrl != null, s"Resource not found: $path")
    new File(resourceUrl.getPath)
  }
}
