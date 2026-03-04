#include "../src/model/event/SingleEvent.h"
#include "../src/model/event/RecurringEvent.h"
#include <cassert>

int main() {
    SingleEvent single1;
    single1.name = "single1";
    single1.note = "note1";
    single1.location = "location1";
    single1.setDate(Date("1.1.2000 12:00"), Date("2.1.2000 12:00"));
    single1.setTags("tag1, tag2 , tag3");
    single1.setParticipants("participant1, participant2, participant3");
    assert(single1.evaluateToken("TAG tag1"));
    assert(single1.evaluateToken("TAG tag2"));
    assert(single1.evaluateToken("TAG tag3"));
    assert(!single1.evaluateToken("TAG tag4"));
    assert(single1.evaluateExpression("TAG tag1"));
    assert(single1.evaluateExpression("TAG tag2"));
    assert(single1.evaluateExpression("TAG   tag3"));
    assert(single1.evaluateExpression("TAG tag1 OR TAG tag1"));
    assert(single1.evaluateExpression("TAG tag1 AND TAG tag1"));
    assert(single1.evaluateExpression("TAG tag1 AND TAG tag2"));
    assert(single1.evaluateExpression("TAG tag1 AND TAG tag2 AND TAG tag3"));
    assert(!single1.evaluateExpression("TAG tag1 AND TAG tag2 AND TAG tag3 AND TAG tag4"));
    assert(single1.evaluateExpression("TAG tag1 AND TAG tag2 OR TAG tag3"));
    assert(single1.evaluateExpression("TAG tag1 OR TAG tag2 AND TAG tag3"));
    assert(single1.evaluateExpression("TAG tag1 OR TAG tag2 OR TAG tag3"));


    assert(single1.evaluateToken("CONTAINS participant1"));
    assert(single1.evaluateToken("CONTAINS participant2"));
    assert(single1.evaluateToken("CONTAINS participant3"));
    assert(!single1.evaluateToken("CONTAINS participant4"));


    assert(single1.evaluateToken("NAME single1"));
    assert(single1.evaluateToken("NOTE note1"));
    assert(single1.evaluateToken("LOCATION location1"));

    assert(single1.evaluateToken("UNTIL 2.1.2000 12:00"));
    assert(single1.evaluateToken("UNTIL 3.1.2000 12:00"));
    assert(single1.evaluateToken("AFTER 31.12.1999 12:00"));
    assert(single1.evaluateExpression("UNTIL 2.1.2000 12:00"));
    assert(single1.evaluateExpression("UNTIL 2.1.2000 12:00 AND UNTIL 3.1.2000 12:00"));
    assert(!single1.evaluateToken("AFTER 2.01.2000 12:00"));
    assert(single1.evaluateExpression("UNTIL 2.1.2000 12:00 AND UNTIL 3.1.2000 12:00 OR AFTER 2.01.2000 12:00"));
    assert(!single1.evaluateExpression("UNTIL 2.1.2000 12:00 AND UNTIL 3.1.2000 12:00 AND AFTER 2.01.2000 12:00"));
    assert(!single1.evaluateToken("UNTIL 1.1.2000 12:00"));

    RecurringEvent rec1;
    rec1.setDate(Date("1.1.2000 12:00"), Date("2.1.2000 12:00"));
    rec1.setRepeatIntervalDays(7); // repeat weekly
    rec1.setRepeatCount(5); // repeat 5 times
    rec1.name = "rec1";
    rec1.note = "note1";
    rec1.location = "location1";
    rec1.setTags("tag1, tag2 , tag3");
    rec1.setParticipants("participant1, participant2, participant3");
    assert(rec1.evaluateToken("TAG tag1"));
    assert(rec1.evaluateToken("TAG tag2"));
    assert(rec1.evaluateToken("TAG tag3"));
    assert(!rec1.evaluateToken("TAG tag4"));


    assert(rec1.evaluateToken("CONTAINS participant1"));
    assert(rec1.evaluateToken("CONTAINS participant2"));
    assert(rec1.evaluateToken("CONTAINS participant3"));
    assert(!rec1.evaluateToken("CONTAINS participant4"));


    assert(rec1.evaluateToken("NAME rec1"));
    assert(rec1.evaluateToken("NOTE note1"));
    assert(rec1.evaluateToken("LOCATION location1"));

    assert(rec1.evaluateToken("UNTIL 2.1.2000 12:00"));
    assert(rec1.evaluateToken("UNTIL 3.1.2000 12:00"));
    assert(rec1.evaluateToken("AFTER 31.12.1999 12:00"));
    assert(rec1.evaluateToken("AFTER 2.01.2000 12:00"));
    assert(!rec1.evaluateToken("UNTIL 1.1.2000 12:00"));

    assert(!rec1.evaluateToken("AFTER " + Date(rec1.getStartDate()).addDays(7 * 5).toString()));
    assert(rec1.evaluateToken("UNTIL " + Date(rec1.getStartDate()).addDays(7 * 5).toString()));
    assert(rec1.evaluateToken("AFTER " + Date(rec1.getStartDate()).addDays(7 * 4).toString()));
    assert(rec1.evaluateToken("UNTIL " + Date(rec1.getStartDate()).addDays(7 * 4).toString()));

}