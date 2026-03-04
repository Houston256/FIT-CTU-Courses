package controller.config.mapper

/**
 * Represents an update to the PipelineConfig class
 * @see PipelineConfig
 */
sealed trait ConfigUpdate
case class SetInput(input: InputConfig) extends ConfigUpdate
case class SetTable(table: TableConfig) extends ConfigUpdate
case class AddFilter(filter: FilterConfig) extends ConfigUpdate
case class SetOutput(output: OutputConfig) extends ConfigUpdate

sealed trait InputConfig
case class FileInput(path: String) extends InputConfig
case class RandomInput(seed: Long) extends InputConfig

sealed trait TableConfig
case class PredefinedTable(name: String) extends TableConfig
case class CustomTable(asciiSequence: String) extends TableConfig

sealed trait FilterConfig
case object InvertConfig extends FilterConfig
case class RotateConfig(degrees: Int) extends FilterConfig
case class ScaleConfig(factor: Double) extends FilterConfig

sealed trait OutputConfig
case object ConsoleOutput extends OutputConfig
case class FileOutput(path: String) extends OutputConfig
