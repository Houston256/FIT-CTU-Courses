#include "Popup.h"


void Popup::show() {
    // Create the popup window
    int popup_y = (LINES - m_popupHeight) / 2;
    int popup_x = (COLS - m_popupWidth) / 2;
    WINDOW *popupwin = newwin(m_popupHeight, m_popupWidth, popup_y, popup_x);

    // check that cyan is available
    auto cyanOnBlack = 0;
    if (COLOR_CYAN < COLORS)
        cyanOnBlack = COLOR_PAIR(COLOR_CYAN);

    // Print tile
    wattron(popupwin, cyanOnBlack | A_UNDERLINE | A_BOLD);
    mvwprintw(popupwin, 0, 0, m_title.c_str());
    wattroff(popupwin, cyanOnBlack | A_UNDERLINE | A_BOLD);

    // Add content to the popup window
    for (size_t i = 0; i < m_lines.size(); i++)
        mvwprintw(popupwin, i + 1, 0, m_lines[i].c_str());

    // Print the bottom line
    wattron(popupwin, A_ITALIC | A_BOLD);
    mvwprintw(popupwin, m_popupHeight - 1, 0, "Press any key to continue.");
    wattroff(popupwin, A_ITALIC | A_BOLD);

    // Refresh the main window and display the popup window
    refresh();
    wrefresh(popupwin);

    // Wait for user input
    getch();

    // Clear the popup window
    werase(popupwin);

    // Clean up and exit
    delwin(popupwin);
}

Popup::Popup(const std::vector<std::string> &lines, std::string title)
        : m_title(std::move(title)), m_lines(lines) {
    m_popupHeight = static_cast<int>(m_lines.size()) + 2;
    for (const auto &line: m_lines) {
        m_popupWidth = std::max(m_popupWidth, static_cast<int>(line.size()) + 2);
    }
}
