#include "MainMenuView.h"
#include "../../model/Constants.h"

void MainMenuView::draw() {
    curs_set(0);
    m_menu.show();
    refresh();
}

ViewState MainMenuView::handleKey(int key_pressed) {
    switch (key_pressed) {
        case KEY_UP:
            m_menu.sendReq(REQ_PREV_ITEM);
            wrefresh(m_menuWin);
            refresh();
            return ViewState::NO_ACTION;
        case KEY_DOWN:
            m_menu.sendReq(REQ_NEXT_ITEM);
            wrefresh(m_menuWin);
            refresh();
            return ViewState::NO_ACTION;
        case ENTER_COMPLETE:
            switch (m_menu.getSelectedIndex()) {
                case 0:
                    return ViewState::VIEW_CALENDAR;
                case 1:
                    return ViewState::IOView;
                case 2:
                    return ViewState::EDIT_CALENDAR;
                case 3:
                    return ViewState::EXIT;
                default: // should not happen
                    return ViewState::NO_ACTION;
            }
        default:
            return ViewState::NO_ACTION;
    }
}

MainMenuView::MainMenuView()
        : m_menuWin(newwin(LINES, COLS, 0, 0)),
          m_menu(options, {"", "", "", ""}, m_menuWin) {
    m_menu.postMenu();
    m_menu.show();
}
