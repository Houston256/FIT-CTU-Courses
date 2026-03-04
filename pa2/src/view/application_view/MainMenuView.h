#pragma once

#include "BaseView.h"
#include "../wrapper/Menu.h"

/**
 * Manages the main window consisting of 4 options
 */
class MainMenuView : public BaseView {
protected:
    const std::vector<std::string> options = {"View calendar", "Import/Export", "Edit calendar", "Exit"};
    WINDOW *m_menuWin;
    Menu m_menu;
public:
    MainMenuView();

    ~MainMenuView() override {
        delwin(m_menuWin);
    }

    ViewState handleKey(int key_pressed) override;

    void draw() override;
};
