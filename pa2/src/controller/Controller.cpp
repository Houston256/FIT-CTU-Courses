#include    "Controller.h"
#include "../view/application_view/EditView.h"
#include "../view/application_view/IOView.h"
#include "../view/calendar_view_controller/CalendarViewController.h"
#include "../model/Constants.h"
#include <memory>
#include "../exceptions/UserAborted.h"

Controller::Controller(const std::string &savePath) : m_save_path(savePath), m_parser(savePath) {
    setup_scr();
    curr_view_ptr = std::make_shared<MainMenuView>();
    try {
        calendar_ptr = std::make_shared<Calendar>(m_parser.read("init.cal"));
    } catch (...) {
        calendar_ptr = std::make_shared<Calendar>();
    }
}

void Controller::handleInput(int key_pressed) {
    // Most common input first
    if (key_pressed == ERR) {
        return;
    } else if (key_pressed == CTRL_D) {
        throw UserAborted();
    } else {
        // Other input, handled by child classes
        switch (curr_view_ptr->handleKey(key_pressed)) {
            case ViewState::NO_ACTION:
                break;
            case ViewState::MAIN_MENU:
            case ViewState::BACK:
                curr_view_ptr = std::make_shared<MainMenuView>();
                break;
            case ViewState::VIEW_CALENDAR:
                curr_view_ptr = std::make_shared<CalendarViewController>(calendar_ptr);
                break;
            case ViewState::IOView:
                curr_view_ptr = std::make_shared<IOView>(calendar_ptr, m_parser);
                break;
            case ViewState::EDIT_CALENDAR:
                curr_view_ptr = std::make_shared<EditView>(calendar_ptr);
                break;
            case ViewState::EXIT:
            default:
                throw UserAborted();
        }
    }
}

void Controller::setup_scr() {
    initscr(); //initialize ncurses
    noecho(); // don't echo input
    raw(); // don't wait for ENTER
    keypad(stdscr, TRUE); // enable special keys
    ESCDELAY = 0; // don't wait for another key after pressing esc

    start_color();

    for (int i = 0; i < COLORS; ++i) {
        init_pair(i, i, COLOR_BLACK);
    }
}

int Controller::run() {
    setup_scr();
    while (true) {
        try {
            curr_view_ptr->draw();
            int key_pressed = getch();
            handleInput(key_pressed);
            refresh();
        } catch (const UserAborted &e) {
            endwin();
            return 0;
        } catch (const std::exception &e) {
            endwin();
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }
}
