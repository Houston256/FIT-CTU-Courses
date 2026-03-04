import controller.AsciiController


@main def run(args: String*): Unit =
  val controller = AsciiController()
  val res = controller.run(args.toList)
  res match {
    case Left(err) => println("Encountered error: " + err.message)
    case _ => ()
  }