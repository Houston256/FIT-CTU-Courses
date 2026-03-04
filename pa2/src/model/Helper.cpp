#include "Helper.h"
#include <algorithm>

std::string Helper::ltrim(const std::string &str) {
    std::string tmp;
    auto firstNotSpace = std::find_if_not(str.begin(), str.end(), [](int ch) {
        return std::isspace(ch);
    });
    std::copy(firstNotSpace, str.end(), std::back_inserter(tmp));
    return tmp;
}

std::string Helper::rtrim(const std::string &str) {
    std::string tmp;
    auto lastNotSpace = std::find_if_not(str.rbegin(), str.rend(), [](int ch) {
        return std::isspace(ch);
    });
    std::copy(str.begin(), lastNotSpace.base(), std::back_inserter(tmp));
    return tmp;
}

std::vector<std::string> Helper::split(const std::string &str, const std::string &delimiter) {
    std::vector<std::string> tokens;
    size_t delPos;
    std::string token;
    size_t prevDelPos = 0;
    while ((delPos = str.find(delimiter, prevDelPos)) != std::string::npos) {
        token = str.substr(prevDelPos, delPos - prevDelPos);
        tokens.emplace_back(token);
        prevDelPos = delPos + delimiter.length();
    }
    token = str.substr(prevDelPos, str.length() - prevDelPos);
    tokens.emplace_back(token);
    return tokens;
}

bool Helper::contains(const std::vector<std::string> &vec, const std::string &str) {
    return std::find(vec.begin(), vec.end(), str) != vec.end();
}

std::string
Helper::join(const std::vector<std::string>::const_iterator &begin, const std::vector<std::string>::const_iterator &end,
             const std::string &delimiter) {
    std::string res;
    if (begin == end) { return res; }
    for (const auto &x: std::vector<std::string>(begin, end)) {
        res += x + delimiter;
    }
    return res.substr(0, res.length() - delimiter.length());
}

void Helper::printInMiddle(WINDOW *win, int yPos, const std::string &str, chtype color) {
    auto maxX = getmaxx(win);
    wattron(win, color);
    mvwprintw(win, yPos, (maxX / 2) - (static_cast<int>(str.size()) / 2), "%s", str.c_str());
    wattroff(win, color);
    wrefresh(win);
    refresh();
}

