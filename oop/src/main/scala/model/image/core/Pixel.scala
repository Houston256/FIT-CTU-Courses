package model.image.core

sealed trait Pixel

case class RGBPixel(red: Int, green: Int, blue: Int) extends Pixel

case class GrayPixel(value: Int) extends Pixel

case class ASCIIPixel(value: Char) extends Pixel
