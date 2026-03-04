#include <cstring>
#include "IOView.h"
#include "../../model/Constants.h"

ViewState IOView::handleKey(int key_pressed) {
    refresh();
    switch (key_pressed) {
        case ESC:
            return ViewState::BACK;
        case KEY_F(1):
            return save();
        case KEY_F(2):
            return load();
        case KEY_F(3):
            return saveSearch();
        case KEY_F(9):
            m_calendar->clear();
            formObj.printStatus("Entire calendar cleared!");
            return ViewState::NO_ACTION;
        default:
            formObj.handleKey(key_pressed);
            return ViewState::NO_ACTION;
    }
}

ViewState IOView::save() {
    std::string filename = formObj.getFieldValue(0);
    if (filename.empty()) {
        formObj.printStatus("Filename must be set");
        return ViewState::NO_ACTION;
    }
    try {
        m_parser.write(filename, m_calendar);
        formObj.printStatus("Calendar saved to \"" + filename + "\"");
        return ViewState::NO_ACTION;
    } catch (const std::exception &e) {
        formObj.printStatus("Problem with saving: " + std::string(e.what()));
        return ViewState::NO_ACTION;
    }
}

ViewState IOView::load() {
    std::string filename = formObj.getFieldValue(0);
    if (filename.empty()) {
        formObj.printStatus("Filename must be set");
        return ViewState::NO_ACTION;
    }
    try {
        auto events = m_parser.read(filename);
        Calendar newCalendar(events);
        m_calendar->clear();
        m_calendar->addEvent(events);
        formObj.printStatus("Calendar loaded " + std::to_string(events.size()) + " events from \"" + filename + "\"");
        return ViewState::NO_ACTION;
    } catch (const std::exception &e) {
        formObj.printStatus("Problem with loading: " + std::string(e.what()));
        return ViewState::NO_ACTION;
    }
}

ViewState IOView::saveSearch() {
    std::string filename = formObj.getFieldValue(0);
    if (filename.empty()) {
        formObj.printStatus("Filename must be set");
        return ViewState::NO_ACTION;
    }
    std::string search = formObj.getFieldValue(1);
    if (search.empty()) {
        formObj.printStatus("Search must be set");
        return ViewState::NO_ACTION;
    }
    try {
        auto events = m_calendar->searchEvents(search);
        m_parser.write(filename, events);
        formObj.printStatus("Search saved to \"" + filename + "\"");
        return ViewState::NO_ACTION;
    } catch (const std::exception &e) {
        formObj.printStatus("Problem with saving: " + std::string(e.what()));
        return ViewState::NO_ACTION;
    }
}

IOView::IOView(const std::shared_ptr<Calendar> &calendar, const Parser &parser): m_calendar(calendar), m_parser(parser) {
    clear(); // clear the screen
    formObj.draw();
    mvprintw(
            2 * formObj.getNumberOfFields() + 6, 1,
            "_______________________________________________________________________________________________________\n"
            "| Keybindings | Action                                      | Which fields should be filled            |\n"
            "| F1          | save entire calendar                        | filename                                 |\n"
            "| F2          | load entire calendar                        | filename                                 |\n"
            "| F3          | save events that match search               | filename, search                         |\n"
            "| F9          | clear entire calendar                       | N/A                                      |\n"
            "| ESC         | cancel                                      | N/A                                      |\n"
            "|_____________|_____________________________________________|__________________________________________|\n"
    );
    refresh();
}
