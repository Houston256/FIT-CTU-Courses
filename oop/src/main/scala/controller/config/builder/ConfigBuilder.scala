package controller.config.builder

import controller.config.PipelineConfig
import controller.config.mapper.ConfigUpdate
import model.core.errors.AppError
import model.image.core.Pixel

/**
 * Class responsible for building a PipelineConfig based on supplied updates
 * @tparam T type of input image
 * @see PipelineConfig
 * @see ConfigUpdate
 */
trait ConfigBuilder[T <: Pixel] {
  /**
   * Build PipelineConfig from updates
   * @param updates list of updates
   * @return Either error if updates are invalid or PipelineConfig
   */
  def build(updates: Seq[ConfigUpdate]): Either[AppError, PipelineConfig[T]]
}
