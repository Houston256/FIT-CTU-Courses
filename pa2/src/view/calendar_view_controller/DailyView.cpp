#include "DailyView.h"


DailyView::DailyView(std::shared_ptr<Calendar> calendar) {
    m_calendar = std::move(calendar);
    m_start = Date::getStartOfDay(Date());
    m_end = Date::getEndOfDay(m_start);
    auto menu_width = COLS - (COLS / 4);
    auto menu_height = LINES - 10;
    auto startX = (COLS / 2) - (menu_width / 2);
    m_menuWin = newwin(menu_height, menu_width, 5, startX);
    updateEvents();
    updateMenu();
}

ViewCalendarState DailyView::handleKey(int key_pressed) {
    auto prevDate = m_selectedDay;
    if (key_pressed == ESC) {
        return ViewCalendarState::BACK;
    } else if (key_pressed == KEY_RIGHT) {
        m_selectedDay.addDays(1);
    } else if (key_pressed == KEY_LEFT) {
        m_selectedDay.addDays(-1);
    } else if (key_pressed == KEY_DOWN) {
        m_menu->sendReq(REQ_NEXT_ITEM);
    } else if (key_pressed == KEY_UP) {
        m_menu->sendReq(REQ_PREV_ITEM);
    } else if (key_pressed == 10 || key_pressed == 13) {
        showPopup();
    }
    if (prevDate != m_selectedDay) {
        m_start = Date::getStartOfDay(m_selectedDay);
        m_end = Date::getEndOfDay(m_selectedDay);
        updateEvents();
        updateMenu();
    }
    return ViewCalendarState::NO_ACTION;
}

void DailyView::draw() {
    clear();
    Helper::printInMiddle(stdscr, 0, "Daily View", 0);
    Helper::printInMiddle(stdscr, 1, "Date: " + m_selectedDay.toString("%d.%m.%Y"), 0);
    Helper::printInMiddle(stdscr, LINES - 1, "Press ESC to go back", COLOR_PAIR(1));
    m_menu->postMenu();
    m_menu->show();
    refresh();
}

void DailyView::updateEvents() {
    m_events = m_calendar->searchEvents(m_start, m_end);
}

void DailyView::updateMenu() {
    std::vector<std::string> labels;
    std::vector<std::string> desc;
    for (const auto &event: m_events) {
        labels.emplace_back("[" + std::to_string(event->id) + "]");
        desc.emplace_back(event->name);
    }
    m_menu = std::make_unique<Menu>(labels, desc, m_menuWin);
    m_menu->postMenu();
}

DailyView::~DailyView() {
    delwin(m_menuWin);

}

void DailyView::showPopup() {
    if (m_events.empty()) {
        return;
    }
    auto &event = m_events[m_menu->getSelectedIndex()];
    event->show().show();
}