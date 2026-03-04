#pragma once

#include "BaseCalendarView.h"
#include "../wrapper/Menu.h"
#include "../../model/Constants.h"

/**
 * View that shows events one day at a time
 */
class DailyView : public BaseCalendarView {
public:
    explicit DailyView(std::shared_ptr<Calendar> calendar);

    ~DailyView() override;

    ViewCalendarState handleKey(int key_pressed) override; //returns next State

    void draw() override;

private:
    /**
     * @brief Updates vector of events for current day
     */
    void updateEvents() final;

    /**
     * @brief Updates menu based on vector of events
     */
    void updateMenu() final;

    /**
     * @brief Shows popup with details of selected event
     */
    void showPopup() final;

    std::vector<std::shared_ptr<BaseEvent>> m_events;
    std::unique_ptr<Menu> m_menu;
    WINDOW *m_menuWin;
};
