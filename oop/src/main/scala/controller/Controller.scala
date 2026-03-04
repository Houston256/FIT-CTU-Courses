package controller

import model.core.errors.AppError

/**
 * This class should control te whole flow of the application.
 */
trait Controller {
  /**
   * Run the application
   * @param args list of arguments
   * @return Either error or nothing
   */
  def run(args: List[String]): Either[AppError, Unit]
}
