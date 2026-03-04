#pragma once

#include <string>
#include <vector>
#include <ncurses.h>
#include <curses.h>

/**
 * Class consisting of helper methods used throughout the whole codebase
 */
class Helper {
public:
    static std::string ltrim(const std::string &str);

    static std::string rtrim(const std::string &str);

    static std::string trim(const std::string &str) { return ltrim(rtrim(str)); }

    static std::vector<std::string> split(const std::string &str, const std::string &delimiter);

    static bool contains(const std::vector<std::string> &vec, const std::string &str);

    static std::string join(const std::vector<std::string>::const_iterator &begin,
                            const std::vector<std::string>::const_iterator &end,
                            const std::string &delimiter);


    static void printInMiddle(WINDOW *win, int yPos, const std::string &str, chtype color);
};
