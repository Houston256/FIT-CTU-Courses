package model.image.io.errors

case class InvalidFile(fileName: String) extends IOError {
  override def message: String = s"File named $fileName is invalid"
}
