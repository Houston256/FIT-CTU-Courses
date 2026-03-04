#include "CalendarViewController.h"
#include "DailyView.h"
#include "WeeklyView.h"
#include "MonthlyView.h"
#include "../../model/Constants.h"
#include <memory>
#include <utility>

CalendarViewController::CalendarViewController(std::shared_ptr<Calendar> calendar) : m_calendar(std::move(calendar)) {
    clear();
    m_currentCalendarView = nullptr;
    m_menu.postMenu();
    m_menu.show();
    refresh();
}

ViewState CalendarViewController::handleKey(int key_pressed) {
    if (m_currentCalendarView == nullptr) {
        switch (key_pressed) {
            case ESC:
                return ViewState::BACK;
            case ENTER_COMPLETE:
                switch (m_menu.getSelectedIndex()) {
                    case 0:
                        m_currentCalendarView = std::make_shared<DailyView>(m_calendar);
                        return ViewState::NO_ACTION;
                    case 1:
                        m_currentCalendarView = std::make_shared<WeeklyView>(m_calendar);
                        return ViewState::NO_ACTION;
                    case 2:
                        m_currentCalendarView = std::make_shared<MonthlyView>(m_calendar);
                        return ViewState::NO_ACTION;
                    default: // should not happen
                        return ViewState::NO_ACTION;
                }
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
            default:
                return ViewState::NO_ACTION;
        }
    } else {
        switch (m_currentCalendarView->handleKey(key_pressed)) {
            case ViewCalendarState::BACK:
                m_currentCalendarView = nullptr;
                clear();
                m_menu.show();
                return ViewState::NO_ACTION;

            case ViewCalendarState::VIEW_DAY:
                m_currentCalendarView = std::make_shared<DailyView>(m_calendar);
                return ViewState::NO_ACTION;

            case ViewCalendarState::VIEW_WEEK:
                m_currentCalendarView = std::make_shared<WeeklyView>(m_calendar);
                return ViewState::NO_ACTION;

            case ViewCalendarState::VIEW_MONTH:
                m_currentCalendarView = std::make_shared<MonthlyView>(m_calendar);
                return ViewState::NO_ACTION;

            case ViewCalendarState::EDIT_CALENDAR:
                return ViewState::EDIT_CALENDAR;

            case ViewCalendarState::SEARCH_CALENDAR:
                return ViewState::IOView;

            case ViewCalendarState::EXIT:
                return ViewState::EXIT;
            default:
                return ViewState::NO_ACTION;
        }
    }
}

void CalendarViewController::draw() {
    curs_set(0);
    if (m_currentCalendarView) {
        m_currentCalendarView->draw();
    }
}

CalendarViewController::~CalendarViewController() {
    delwin(m_menuWin);

}

