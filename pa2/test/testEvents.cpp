#include "../src/model/event/BaseEvent.h"
#include "../src/model/event/SingleEvent.h"
#include "../src/model/event/RecurringEvent.h"
#include "../src/model/wrapper/Date.h"
#include <cassert>

int main() {
    Date now, hourLater(now);
    hourLater.addHours(1);

    SingleEvent single1;
    single1.setDate(now, hourLater);
    assert(single1.getStartDate() == now);
    assert(single1.getEndDate() == hourLater);

    single1.name = "Study session";
    single1.note = "Interesting event";

    assert(single1.isCollision(&single1));
    assert(single1.isCollision(now, hourLater));

    // subsequent events are allowed and shouldn't collide
    assert(!single1.isCollision(hourLater, now.addHours(2)));

    auto test_event = SingleEvent{
            1,
            "name",
            "note",
            "location",
            "10.6.2022 12:30",
            "19.6.2022 13:30",
            "1, 2, 3, 4, dsjgdf, Jack, Jeff",
            "tag1, tag2, tag3"
    };

    auto test_event2 = SingleEvent{
            3,
            "Some other name",
            "wooohoooooooo",
            "Wooow",
            "19.6.2022 13:30",
            "19.6.2022 14:00",
            "1, 2, 3, 4, dsjgdf, Jack, Jeff",
            "tag1, tag2, tag3"
    };

    SingleEvent newEvent;
    Date date1("24.05.2023 22:08"), date2("24.05.2023 23:08");
    newEvent.setDate(date1, date2);
    SingleEvent newEvent2(0, "", "", "", "24.05.2023 22:08", "24.05.2023 23:08", "", "");
    assert(newEvent.getStartDate() == newEvent2.getStartDate());
    assert(newEvent.getEndDate() == newEvent2.getEndDate());
    // Collisions
    assert(!test_event.isCollision(&test_event2));
    assert(test_event.isCollision(test_event2.getStartDate().addMinutes(-1), test_event2.getEndDate()));
    auto rec_event1 = RecurringEvent(
            1,
            "name",
            "note",
            "location",
            "9.6.2022 12:30",
            "9.6.2022 13:30",
            "Jeff, Jack, Richard, Oliver", "",
            1,
            10
    );
    assert(rec_event1.isCollision(Date("19.6.2022 12:30"), Date("19.6.2022 13:30")));
    assert(!rec_event1.isCollision(Date("19.6.2022 13:30"), Date("19.6.2022 14:30")));
    std::vector<std::string> rec_event1Attendees{"Jeff", "Jack", "Richard", "Oliver"};
    assert(rec_event1.id == 1);
    assert(rec_event1.name == "name");
    assert(rec_event1.note == "note");
    assert(rec_event1.location == "location");
    assert(rec_event1.getStartDate().toString() == "09.06.2022 12:30");
    assert(rec_event1.getEndDate().toString() == "09.06.2022 13:30");
    assert(rec_event1.getParticipants() == rec_event1Attendees);

    assert(rec_event1.isCollision(&rec_event1));
    assert(rec_event1.isCollision(&test_event));
    assert(!rec_event1.isCollision(&test_event2));
    // rec with rec
    // this should collide on 15.6.2022
    auto rec_event2 = RecurringEvent(
            2,
            "",
            "",
            "",
            "1.6.2022 12:30",
            "1.6.2022 13:30",
            "",
            "",
            7,
            10
    );
    assert(rec_event1.isCollision(&rec_event2));
    return 0;
}