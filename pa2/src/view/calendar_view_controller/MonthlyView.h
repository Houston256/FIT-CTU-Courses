#pragma once

#include "BaseCalendarView.h"
#include "../../model/Constants.h"
#include "../wrapper/Menu.h"
#include <menu.h>

/**
 * View that shows events for an entire month
 */
class MonthlyView : public BaseCalendarView {
public:
    explicit MonthlyView(std::shared_ptr<Calendar> calendar);

    ~MonthlyView() override;

    ViewCalendarState handleKey(int key_pressed) override;

    void draw() override;

protected:

    /**
     * @brief Displays a month widget
     * @details selected day is highlighted and days with events are colored
     */
    void displayMonth();

    /**
     * @brief Displays a header with month and year
     */
    void displayHeader();

    /**
     * @brief Updates vector of events for current month
     */
    void updateEvents() final;

    /**
     * @brief Shows menu based on events
     */
    void displayEvents();

    /**
     * Initializes menu with events in selected day
     */
    void updateMenu() final;

    /**
     * Shows popup with details of selected event
     */
    void showPopup() final;

    std::vector<std::vector<std::shared_ptr<BaseEvent>>> m_events;
    std::vector<std::string> m_menuLabels, m_menuDescriptions;
    std::unique_ptr<Menu> m_menu;
    WINDOW *m_menuWin;
};
