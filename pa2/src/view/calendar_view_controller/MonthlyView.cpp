#include <cstring>
#include "MonthlyView.h"

void MonthlyView::displayHeader() {
    if (COLOR_YELLOW < COLORS) {
        attron(COLOR_PAIR(COLOR_YELLOW));
    }
    mvprintw(0, COLS / 2, "Monthly View");
    attroff(COLOR_PAIR(COLOR_YELLOW));

    attron(A_UNDERLINE | A_BOLD);
    mvprintw(1, 0, "%s", (std::string(months[m_start.getMonth() - 1]) + " " + std::to_string(m_start.getYear())).c_str());
    attroff(A_UNDERLINE | A_BOLD);

    mvprintw(3, 0, "Press ESC to go back");
    mvprintw(4, 0,
             "Press LEFT/RIGHT/UP/DOWN to change days, PageUp/PageDown to select event in day, enter to show that event");
    mvhline(5, 0, '-', COLS);
}

void MonthlyView::displayMonth() {
    auto displayStart = Date::getStartOfWeek(m_start);
    auto displayEnd = Date::getEndOfWeek(m_end);
    auto row = 7;
    auto selectedDayAttr = A_STANDOUT | A_BLINK;
    auto dayWithEventsAttr = A_BOLD;
    auto redOnBlack = 0;
    if (COLOR_RED < COLORS) {
        dayWithEventsAttr |= COLOR_PAIR(COLOR_RED);
        redOnBlack = COLOR_PAIR(COLOR_RED);
    }
    auto otherMonthAttr = A_DIM;
    for (size_t i = 0; i < 7; ++i) {
        mvprintw(6, i * 4, "%s", daysShort[i]);
    }
    attron(A_BOLD);
    while (displayStart < displayEnd) {
        if ((m_start.getMonth() == displayStart.getMonth()) && !m_events[displayStart.getDay() - 1].empty()) {
            attron(dayWithEventsAttr);
        }
        if (displayStart.getMonth() != m_start.getMonth()) {
            auto tmpStartOfDay = Date::getStartOfDay(displayStart);
            auto tmpEndOfDay = Date::getEndOfDay(displayStart);
            if (!m_calendar->searchEvents(tmpStartOfDay, tmpEndOfDay).empty()) {
                attron(redOnBlack);
            }
            attron(otherMonthAttr);
        }
        if (Date::getStartOfDay(displayStart) == Date::getStartOfDay(m_selectedDay)) {
            attron(selectedDayAttr);
        }
        mvprintw(row, displayStart.getWday() * 4, "%3d", displayStart.getDay());
        attroff(otherMonthAttr);
        attroff(dayWithEventsAttr);
        attroff(selectedDayAttr);

        displayStart.addDays(1);
        if (displayStart.getWday() == 0) {
            row++;
        }
    }
}

MonthlyView::MonthlyView(std::shared_ptr<Calendar> calendar) {
    m_calendar = std::move(calendar);
    m_start = Date::getStartOfMonth(m_start);
    m_end = Date::getEndOfMonth(m_start);
    updateEvents();
    m_menuWin = newwin(LINES - 10, COLS / 2 - 10, 7, COLS / 2);
    keypad(m_menuWin, TRUE);
    updateMenu();
}

ViewCalendarState MonthlyView::handleKey(int key_pressed) {
    auto prevDay = m_selectedDay;
    if (key_pressed == ESC) {
        return ViewCalendarState::BACK;
    } else if (key_pressed == KEY_RIGHT) {
        m_selectedDay.addDays(1);
    } else if (key_pressed == KEY_LEFT) {
        m_selectedDay.addDays(-1);
    } else if (key_pressed == KEY_UP) {
        m_selectedDay.addDays(-7);
    } else if (key_pressed == KEY_DOWN) {
        m_selectedDay.addDays(7);
    } else if (key_pressed == KEY_NPAGE) {
        m_menu->sendReq(REQ_DOWN_ITEM);
        wrefresh(m_menuWin);
    } else if (key_pressed == KEY_PPAGE) {
        m_menu->sendReq(REQ_UP_ITEM);
        wrefresh(m_menuWin);
    } else if (key_pressed == 10 || key_pressed == 13) { // enter
        showPopup();
    }

    if (m_selectedDay.getMonth() != m_start.getMonth()) {
        m_start = Date::getStartOfMonth(m_selectedDay);
        m_end = Date::getEndOfMonth(m_selectedDay);
        updateEvents();
    }

    if (Date::getStartOfDay(prevDay) != Date::getStartOfDay(m_selectedDay)) {
        updateMenu();
    }
    return ViewCalendarState::NO_ACTION;
}

void MonthlyView::draw() {
    clear();
    displayHeader();
    displayMonth();
    displayEvents();
    refresh();
}

void MonthlyView::updateEvents() {
    m_events.clear();
    for (auto i = m_start; i <= m_end; i.addDays(1)) {
        std::vector<std::shared_ptr<BaseEvent>> eventsInDay{};
        for (const auto &event_ptr: m_calendar->getEvents()) {
            if (event_ptr->isCollision(Date::getStartOfDay(i), Date::getEndOfDay(i))) {
                eventsInDay.emplace_back(event_ptr);
            }
        }
        m_events.emplace_back(eventsInDay);
    }
}

void MonthlyView::displayEvents() {
    m_menu->postMenu();
    auto tmpTitle = "Events on " + m_selectedDay.toString("%d.%m.%Y");
    Helper::printInMiddle(m_menuWin, 1, tmpTitle, COLOR_PAIR(2));
    m_menu->show();
    wrefresh(m_menuWin);

    refresh();
}

void MonthlyView::updateMenu() {
    m_menuLabels.clear();
    m_menuDescriptions.clear();
    auto &currentEvents = m_events[m_selectedDay.getDay() - 1];
    m_menuLabels.reserve(currentEvents.size());
    m_menuDescriptions.reserve(currentEvents.size());

    for (const auto &currentEvent: currentEvents) {
        m_menuLabels.push_back("[" + std::to_string(currentEvent->id) + "]");
        m_menuDescriptions.emplace_back(currentEvent->name);
    }
    m_menu = std::make_unique<Menu>(m_menuLabels, m_menuDescriptions, m_menuWin);
}


MonthlyView::~MonthlyView() {
    delwin(m_menuWin);

}

void MonthlyView::showPopup() {
    auto &currentDayEvents = m_events[m_selectedDay.getDay() - 1];
    if (currentDayEvents.empty()) { return; }
    currentDayEvents[m_menu->getSelectedIndex()]->show().show();
}
