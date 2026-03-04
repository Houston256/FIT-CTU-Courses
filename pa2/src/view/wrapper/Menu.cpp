#include <stdexcept>
#include "Menu.h"


Menu::Menu(const std::vector<std::string> &labels, const std::vector<std::string> &descriptions, WINDOW *menuWin)
        : m_labels(labels), m_descriptions(descriptions), m_menuWin(menuWin) {
    // validate input
    if (labels.size() != descriptions.size()) {
        throw std::invalid_argument("labels and descriptions must be of same size");
    }
    if (!menuWin) {
        throw std::invalid_argument("menuWin can't be nullptr");
    }
    // prepare items
    m_n_choices = labels.size();
    if (m_n_choices != 0) {
        m_items = (ITEM **) calloc(m_n_choices + 1, sizeof(ITEM *));
        for (size_t i = 0; i < m_n_choices; ++i) {
            m_items[i] = new_item(m_labels[i].c_str(), m_descriptions[i].c_str());
        }
        m_items[m_n_choices] = (ITEM *) nullptr;
        // create menu
        m_menu = new_menu(m_items);
        set_menu_win(m_menu, m_menuWin);
        derivedWindow = derwin(m_menuWin, 0, 0, 3, 1);
        set_menu_sub(m_menu, derivedWindow);
        wrefresh(m_menuWin);
        set_menu_format(m_menu, getmaxy(m_menuWin) - 4, 1);
        set_menu_mark(m_menu, " * ");
        wrefresh(m_menuWin);

        sendReq(REQ_NEXT_ITEM);
        sendReq(REQ_PREV_ITEM);
        wrefresh(m_menuWin);
        refresh();
    }
}

Menu::~Menu() {
    if (m_n_choices != 0) {
        if (m_menu != nullptr) {
            unpostMenu();
            free_menu(m_menu);
        }
        if (derivedWindow != nullptr) {
            delwin(derivedWindow);
            derivedWindow = nullptr;
        }
        m_menu = nullptr;
        for (size_t i = 0; i < m_n_choices; ++i) {
            free_item(m_items[i]);
            m_items[i] = nullptr;
        }
        if (m_items != nullptr) {
            free(m_items);
            m_items = nullptr;
        }
        m_n_choices = 0;
    }
}
