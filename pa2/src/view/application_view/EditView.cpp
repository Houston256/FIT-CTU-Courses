#include <sstream>
#include "EditView.h"
#include "../../model/Constants.h"
#include "../../model/event/SingleEvent.h"
#include "../../model/event/visitor/EditEventVisitor.h"

EditView::EditView(std::shared_ptr<Calendar> calendar) : m_calendar(std::move(calendar)) {
    clear(); // clear the screen
    cbreak(); // don't wait for ENTER
    noecho(); // don't echo input
    keypad(stdscr, TRUE); // enable special keys

    formObject.draw();

    Date now;
    formObject.setFieldValue(4, now.toString());
    formObject.setFieldValue(5, now.addHours(1).toString());

    formObject.setFieldType(0, TYPE_INTEGER);
    formObject.setFieldType(8, TYPE_INTEGER);
    formObject.setFieldType(9, TYPE_INTEGER);

    mvprintw(
            2 * formObject.getNumberOfFields() + 6, 1,
            "_______________________________________________________________________________________________________\n"
            "| Keybindings | Action                                      | Which fields should be filled            |\n"
            "| ENTER       | search by ID (fills fields with attributes) | ID                                       |\n"
            "| F1          | remove event                                | ID                                       |\n"
            "| F2          | edit event                                  | ID, attributes of given event            |\n"
            "| F3          | add single event                            | attributes of single event               |\n"
            "| F4          | add recurring event                         | attributes of recurring event            |\n"
            "| F5          | clear all fields                            | N/A                                      |\n"
            "| F6          | Search calendar with prompt                 | Search                                   |\n"
            "| ESC         | cancel                                      | N/A                                      |\n"
            "|_____________|_____________________________________________|__________________________________________|\n"
    );
}

ViewState EditView::handleKey(int key_pressed) {
    try {
        switch (key_pressed) {
            case ESC:
                return ViewState::BACK;
            case ENTER_COMPLETE:
                return searchByID();
            case KEY_F(1):
                return deleteEvent();
            case KEY_F(2):
                return editEvent();
            case KEY_F(3):
                return addSingleEvent();
            case KEY_F(4):
                return addRecurringEvent();
            case KEY_F(5): // clear form
                formObject.clearFields();
                return ViewState::NO_ACTION;
            case KEY_F(6): // search by prompt
                return search();
            default:
                formObject.handleKey(key_pressed);
                return ViewState::NO_ACTION;
        }
    } catch (const std::exception &e) {
        formObject.printStatus(e.what());
        return ViewState::NO_ACTION;
    }
}

void EditView::draw() {
    curs_set(1);
    refresh();
}

ViewState EditView::searchByID() {
    size_t id;
    std::string eventStr;
    std::vector<std::string> attributes;
    std::shared_ptr<BaseEvent> event;
    // check if ID is set
    if (formObject.getFieldValue(0).empty()) {
        formObject.printStatus("ID must be set");
        return ViewState::NO_ACTION;
    }
    // check if ID is valid
    try {
        id = formObject.getUnsignedValue(0);
    } catch (const std::exception &e) {
        formObject.printStatus("Problem with id: " + std::string(e.what()));
        return ViewState::NO_ACTION;
    }
    // search for event with given ID
    if (!(event = m_calendar->searchEvents(id))) {
        formObject.printStatus("Event with ID " + std::to_string(id) + " not found");
        return ViewState::NO_ACTION;
    }
    formObject.clearFields();
    formObject.setFieldValue(0, std::to_string(id));
    formObject.printStatus("Event with ID " + std::to_string(id) + " found");
    // print event attributes
    event->print(formObject);
    return ViewState::NO_ACTION;
}

ViewState EditView::addSingleEvent() {
    std::shared_ptr<BaseEvent> event_ptr;
    EditEventVisitor visitor;
    try {
        visitor.setAttributes(formObject.getFieldValues());
        event_ptr = std::make_shared<SingleEvent>();
        event_ptr->accept(visitor);
        m_calendar->addEvent(event_ptr);
        formObject.printStatus(
                "Single event added (Assigned ID: " + std::to_string(event_ptr->id) + ")");
    } catch (std::exception &e) {
        formObject.printStatus("Failed to add single event, because: " + std::string(e.what()));
        return ViewState::NO_ACTION;
    }
    return ViewState::NO_ACTION;
}

ViewState EditView::deleteEvent() {
    size_t id;
    id = formObject.getUnsignedValue(0);
    if (m_calendar->removeEvent(id)) {
        formObject.printStatus("Event with ID " + std::to_string(id) + " removed");
    } else {
        formObject.printStatus("Event with ID " + std::to_string(id) + " not found");
    }
    return ViewState::NO_ACTION;
}

ViewState EditView::editEvent() {
    size_t id;
    std::shared_ptr<BaseEvent> event_ptr;
    std::shared_ptr<BaseEvent> event_ptr_clone;
    EditEventVisitor visitor;
    // check if ID is set
    if (formObject.getFieldValue(0).empty()) {
        formObject.printStatus("ID must be set");
        return ViewState::NO_ACTION;
    }
    try {
        id = formObject.getUnsignedValue(0);
        // check if event exists
        if (!(event_ptr = m_calendar->searchEvents(id))) {
            formObject.printStatus(
                    "Event with ID " + std::to_string(id) + " not found, edit can't be performed.");
            formObject.clearField(0);
            return ViewState::NO_ACTION;
        }
        // init visitor
        visitor.setAttributes(formObject.getFieldValues());
        // clone original event
        event_ptr_clone = event_ptr->clone();
        // remove original event
        m_calendar->removeEvent(id);
        // try adding edited event
        event_ptr_clone->accept(visitor);
        m_calendar->addEvent(event_ptr_clone, event_ptr_clone->id);
    } catch (const std::exception &e) {
        // if something went wrong, add original event back
        m_calendar->addEvent(event_ptr, event_ptr->id);
        formObject.printStatus("Edit can't be performed, because: " + std::string(e.what()));
        return ViewState::NO_ACTION;
    }
    formObject.printStatus("Edit successful!");
    return ViewState::NO_ACTION;
}

ViewState EditView::addRecurringEvent() {
    std::shared_ptr<BaseEvent> event_ptr;
    EditEventVisitor visitor;
    try {
        visitor.setAttributes(formObject.getFieldValues());
        event_ptr = std::make_shared<RecurringEvent>();
        event_ptr->accept(visitor);
        m_calendar->addEvent(event_ptr);
        formObject.printStatus(
                "Recurring event added (Assigned ID: " + std::to_string(event_ptr->id) + ")");
    } catch (std::exception &e) {
        formObject.printStatus("Failed to add recurring event: " + std::string(e.what()));
        return ViewState::NO_ACTION;
    }
    return ViewState::NO_ACTION;
}

ViewState EditView::search() {
    std::vector<std::string> res_id;
    auto prompt = formObject.getFieldValue(10);
    prompt = Helper::trim(prompt);
    std::vector<std::shared_ptr<BaseEvent>> res;
    try {
        res = m_calendar->searchEvents(prompt);
    } catch (const std::exception &e) {
        formObject.printStatus("Search failed: " + std::string(e.what()));
        return ViewState::NO_ACTION;
    }
    if (res.empty()) {
        formObject.printStatus("No events found for current prompt.");
        return ViewState::NO_ACTION;
    }
    res_id.reserve(res.size());
    for (const auto &event: res) {
        res_id.emplace_back(std::to_string(event->id));
    }
    formObject.printStatus(
            "Found events with ID: " + Helper::join(res_id.begin(), res_id.end(), ", "));
    return ViewState::NO_ACTION;
}
