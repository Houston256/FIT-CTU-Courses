#pragma once

#include "BaseCalendarView.h"
#include "../wrapper/Menu.h"
#include "../../model/Constants.h"

/**
 * View that displays events for a whole week at once
 */
class WeeklyView : public BaseCalendarView {
public:

    explicit WeeklyView(std::shared_ptr<Calendar> calendar);

    ~WeeklyView() override;

    ViewCalendarState handleKey(int key_pressed) override;

    void draw() override;

private:
    void updateEvents() final;

    void updateMenu() final;

protected:
    void showPopup() final;

private:
    std::vector<WINDOW *> m_windows;
    std::vector<Menu> m_menus;
    std::vector<std::vector<std::shared_ptr<BaseEvent>>> m_events;
    int m_paddingX, m_paddingY, m_width, m_height;
};
