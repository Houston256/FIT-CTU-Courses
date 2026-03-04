#pragma once
//#include <ncurses.h>
#include <menu.h>
#include <vector>
#include <string>
#include "../../model/Helper.h"

/**
 * @brief Menu.h wrapper
 */
class Menu {
public:
    Menu() = default;

    Menu(const std::vector<std::string> &labels, const std::vector<std::string> &descriptions, WINDOW *menuWin);

    ~Menu();

    int postMenu() {
        return post_menu(m_menu);
    }

    [[nodiscard]] size_t getSelectedIndex() {
        return item_index(current_item(m_menu));
    }

    int sendReq(int key) {
        return menu_driver(m_menu, key);
    }

    int unpostMenu() {
        return unpost_menu(m_menu);
    }

    // call this method right after posting
    void show() {
        Helper::printInMiddle(m_menuWin, 1, title, COLOR_PAIR(0));

        box(m_menuWin, 0, 0);
        wrefresh(m_menuWin);
        refresh();
    }

    size_t size() const {
        return m_n_choices;
    }

    bool empty() const {
        return m_n_choices == 0;
    }

public:
    std::string title;
private:
    std::vector<std::string> m_labels;
    std::vector<std::string> m_descriptions;
    MENU *m_menu = nullptr;
    ITEM **m_items = nullptr;
    size_t m_n_choices = 0;
    WINDOW *m_menuWin = nullptr, *derivedWindow = nullptr;
};


