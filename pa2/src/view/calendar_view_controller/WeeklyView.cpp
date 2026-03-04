#include "WeeklyView.h"

WeeklyView::WeeklyView(std::shared_ptr<Calendar> calendar) {
    m_calendar = std::move(calendar);
    Date now, start, end;
    start = Date::getStartOfWeek(now);
    end = Date::getEndOfWeek(now);
    m_start = start;
    m_end = end;
    int maxX = getmaxx(stdscr);
    int maxY = getmaxy(stdscr);
    m_paddingX = 1;
    m_paddingY = 4;
    m_width = (maxX - m_paddingX * 2) / 7;
    m_height = (maxY - m_paddingY * 2);
    // init windows
    for (int i = 0; i < 7; i++) {
        m_windows.push_back(newwin(m_height, m_width, m_paddingY, m_paddingX + i * m_width));
    }
    updateEvents();
    updateMenu();

    refresh();
}

ViewCalendarState WeeklyView::handleKey(int key_pressed) {
    if (key_pressed == ESC)
        return ViewCalendarState::BACK;
    else if (key_pressed == KEY_RIGHT)
        m_selectedDay.addDays(1);
    else if (key_pressed == KEY_LEFT)
        m_selectedDay.addDays(-1);
    else if (key_pressed == KEY_DOWN) {
        m_menus[m_selectedDay.getWday()].sendReq(REQ_NEXT_ITEM);
        refresh();
    } else if (key_pressed == KEY_UP) {
        m_menus[m_selectedDay.getWday()].sendReq(REQ_PREV_ITEM);
        refresh();
    } else if (key_pressed == 10 || key_pressed == 13) {
        // enter
        showPopup();
    }
    if (Date::getStartOfWeek(m_selectedDay) != Date::getStartOfWeek(m_start)) {
        m_start = Date::getStartOfWeek(m_selectedDay);
        m_end = Date::getEndOfWeek(m_selectedDay);
        updateEvents();
        updateMenu();
        refresh();
    }

    return ViewCalendarState::NO_ACTION;
}

void WeeklyView::draw() {
    clear();
    mvprintw(0, 0, "Weekly View: RIGHT/LEFT to switch days, UP/DOWN to select event, ENTER to show Popup");
    auto dateCopy = Date(m_start);
    for (int i = 0; i < 7; ++i) {
        if (m_selectedDay.getWday() == i) {
            attron(A_STANDOUT);
        }
        mvprintw(m_paddingY - 2, m_paddingX + i * m_width, week_days[i]);
        mvprintw(m_paddingY - 1, m_paddingX + i * m_width, dateCopy.toString("%d.%m.%Y").c_str());
        attroff(A_STANDOUT);
        dateCopy.addDays(1);
    }
    for (auto &menu: m_menus) {
        menu.postMenu();
        menu.show();
        refresh();
    }
    refresh();
}

WeeklyView::~WeeklyView() {
    for (auto &win: m_windows) {
        if (!win) continue;
        delwin(win);
        win = nullptr;
    }
}

void WeeklyView::updateEvents() {
    m_events.clear();
    for (auto i = m_start; i <= m_end; i.addDays(1)) {
        m_events.push_back(m_calendar->searchEvents(Date::getStartOfDay(i), Date::getEndOfDay(i)));
    }
}

void WeeklyView::updateMenu() {
    m_menus.clear();
    if (m_events.size() != 7) {
        throw std::runtime_error("WeeklyView::updateMenu::can't init all 7 menus");
    }
    if (m_windows.size() != 7) {
        throw std::runtime_error("Windows must be initialized");
    }
    for (const auto &window: m_windows) {
        if (!window) {
            throw std::runtime_error("All windows must be initialized");
        }
    }
    std::vector<std::string> labels;
    std::vector<std::string> descriptions;
    m_menus.reserve(7);
    for (size_t i = 0; i < 7; ++i) {
        labels.clear();
        descriptions.clear();
        for (auto &event: m_events[i]) {
            auto label = "[" + std::to_string(event->id) + "] ";
            labels.emplace_back(label);
            descriptions.emplace_back(event->name);
        }
        m_menus.emplace_back(labels, descriptions, m_windows[i]);
    }
    for (auto &menu: m_menus) {
        menu.postMenu();
    }
}

void WeeklyView::showPopup() {
    auto wday = m_selectedDay.getWday();
    auto &menu = m_menus[wday];
    if (!menu.empty()) {
        auto selectedIdx = menu.getSelectedIndex();
        auto event = m_events[wday][selectedIdx];
        auto pop = event->show();
        pop.show();
    }
}
