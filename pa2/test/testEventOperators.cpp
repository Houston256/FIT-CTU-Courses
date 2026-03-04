#include <cassert>
#include "../src/model/event/SingleEvent.h"
#include "../src/model/event/RecurringEvent.h"

int main() {
    RecurringEvent rec1;
    Date initialStart("1.1.2000 12:00"), initialEnd("2.1.2000 12:00");
    rec1.setDate(initialStart, initialEnd);
    rec1.setRepeatIntervalDays(7); // repeat weekly
    rec1.setRepeatCount(5); // repeat 5 times

    assert(rec1 > initialStart);
    assert(rec1 > initialEnd);
    assert(rec1 > Date("3.1.2000 12:00"));
    assert(rec1 > Date("4.1.2000 12:00"));
    assert(rec1 > Date(initialStart).addDays(7 * 4));
    assert(!(rec1 > Date(initialStart).addDays(7 * 5)));
    assert(rec1 < Date(initialStart).addDays(7 * 6));


    SingleEvent single1;
    single1.setDate(initialStart, initialEnd);
    return 0;
}