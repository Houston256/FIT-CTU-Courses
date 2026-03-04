#include "controller/Controller.h"
#include "../libs/argparse.hpp"

#ifndef TEST

// source: https://github.com/p-ranav/argparse#quick-start
int main(int argc, char *argv[]) {
    // prepare argument parser
    argparse::ArgumentParser program("krausri1");

    program.add_argument("-d", "--directory")
            .help("specify the save/load directory")
            .default_value(std::filesystem::current_path().string());

    // try reading arguments
    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    // save arguments
    auto directory = program.get<std::string>("--directory");

    // validate arguments
    if (!std::filesystem::exists(directory)) {
        std::cerr << "Path \"" << directory << "\" does not exist" << std::endl;
        return 1;
    }

    if (!std::filesystem::is_directory(directory)) {
        std::cerr << "Path \"" << directory << "\" isn't a directory" << std::endl;
        return 1;
    }

    // run program
    Controller controller(directory);
    auto res = controller.run();
    endwin();
    exit_curses(res);
    return res;
}

#endif
