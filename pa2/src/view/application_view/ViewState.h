#pragma once

enum class ViewState {
    MAIN_MENU,
    VIEW_CALENDAR,
    IOView,
    EDIT_CALENDAR,
    EXIT,
    BACK,
    NO_ACTION
};

enum class ViewCalendarState {
    VIEW_DAY,
    VIEW_WEEK,
    VIEW_MONTH,
    EDIT_CALENDAR,
    SEARCH_CALENDAR,
    EXIT,
    BACK,
    NO_ACTION
};