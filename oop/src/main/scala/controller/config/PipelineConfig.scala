package controller.config

import model.image.core.{ASCIIPixel, GrayPixel, Pixel}
import model.image.filter.Filter
import model.image.filter.operator.OperatorSeq
import model.image.io.loader.Loader
import model.image.io.writer.Writer

/**
 * Configuration for a program which intends to convert an image to ASCII art
 * @param imageReader Loader which yields the input image
 * @param toGray Filter for conversion to GrayScale
 * @param grayFilters Filters over GrayScale which should be applied to the grayscale image, in sequence
 * @param finalFilter Filter which is responsible for GrayScale -> ASCII conversion
 * @param imageWriters Sequence of writers
 * @tparam Input type of the input image
 */
case class PipelineConfig[Input <: Pixel](
  imageReader: Loader[Input],
  toGray: Filter[Input, GrayPixel],
  grayFilters: OperatorSeq[GrayPixel],
  finalFilter: Filter[GrayPixel, ASCIIPixel],
  imageWriters: Seq[Writer[ASCIIPixel]]
)
