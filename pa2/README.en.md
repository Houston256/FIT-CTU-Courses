Assignment from ProgTest: Planning Calendar

Calendar management application with the ability to create and search for events.

Implement the following functionality:

    daily, weekly and monthly calendar_ptr views
    create and manage events (time, place, date, participants, ...)
    possibility to set recurring events (once a week, 14 days, ...)
    search for events by name and / or venue
    look for the nearest possible date and the possibility to move events
    exporting and importing events

Use of polymorphism (recommended)

    types of events: mandatory, optional, transferable, ...
    move strategy: closest possible (with restriction), nearest possible with move X other events
    calendar_ptr curr_view_ptr (daily, weekly, monthly)
    formats of exported events

Where do I use polymorphism?

BaseEvent   
- SingleEvent
- RecurringEvent

BaseView
- CalendarViewController
- EditView
- MainMenuView
- IOView

BaseCalendarView
- DailyView
- WeeklyView
- MonthlyView