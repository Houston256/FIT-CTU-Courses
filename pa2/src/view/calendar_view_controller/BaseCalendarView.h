#pragma once

#include "../application_view/ViewState.h"
#include "../../model/Calendar.h"
#include <ncurses.h>
#include <memory>

/**
 * @brief A class which is responsible for displaying the calendar.
 */
class BaseCalendarView {
protected:
    std::shared_ptr<Calendar> m_calendar;
    Date m_start, m_end, m_selectedDay;
public:
    /**
     * @brief BaseCalendarView default destructor.
     */
    virtual ~BaseCalendarView() = default;

    /**
     * @brief handle input from user
     * @param key_pressed last user input
     * @return next ViewCalendarState
     */
    virtual ViewCalendarState handleKey(int key_pressed) = 0;

    /**
     * @brief draw view
     */
    virtual void draw() = 0;
protected:
    /**
     * @brief update events for current date range and save it to desired data structure
     */
    virtual void updateEvents() = 0;

    virtual void showPopup() = 0;

    virtual void updateMenu() = 0;
};