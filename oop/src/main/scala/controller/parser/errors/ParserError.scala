package controller.parser.errors

import model.core.errors.AppError

trait ParserError extends Throwable with AppError
