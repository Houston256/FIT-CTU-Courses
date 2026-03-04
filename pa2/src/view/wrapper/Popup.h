#pragma once

#include <utility>
#include <vector>
#include <string>
#include <ncurses.h>

/**
 * Manages its own window with a message that gets removed after pressing a key.
 */
class Popup {
public:
    Popup() = delete;

    /**
     * Popup constructor
     * @param lines that will be added to the body of popup
     * @param title of popup
     */
    explicit Popup(const std::vector<std::string> &lines, std::string title = "");

    /**
     * @brief shows the popup
     */
    void show();


private:
    int m_popupHeight = 10, m_popupWidth = 40;
    std::string m_title;
    std::vector<std::string> m_lines;
};




