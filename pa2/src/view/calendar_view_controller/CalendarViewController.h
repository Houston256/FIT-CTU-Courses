#pragma once

#include <memory>
#include "../application_view/BaseView.h"
#include "../../model/Calendar.h"
#include "BaseCalendarView.h"
//#include "../wrapper/MenuWrapper.h"
#include "../wrapper/Menu.h"

/**
 * @brief A hybrid view/controller class responsible for managing calendar views.
 */
class CalendarViewController : public BaseView {
protected:
    // calendar with which all views work
    std::shared_ptr<Calendar> m_calendar;
    // current calendar view
    std::shared_ptr<BaseCalendarView> m_currentCalendarView;
    // options which are displayed when user is prompted to choose a calendar view
    std::vector<std::string> m_calendarViewOptions{"Daily", "Weekly", "Monthly"};
    WINDOW *m_menuWin = newwin(LINES, COLS, 0, 0);
    Menu m_menu{{"Daily", "Weekly", "Monthly"}, {"", "", ""}, m_menuWin};
    // previous ncurses cursor setting, to ensure that it is restored when this class gets destructed
public:
    /**
     * @brief CalendarViewController Constructor.
     * @param calendar calendar to be saved
     */
    explicit CalendarViewController(std::shared_ptr<Calendar> calendar);

    /**
     * @brief Virtual default constructor.
     */
    ~CalendarViewController() override;

    /**
     * @brief Handle a key press
     * @param key_pressed key which was pressed
     */
    ViewState handleKey(int key_pressed) override;

    /**
     * @brief Update view
     */

    void draw() override;
};
