#include <cassert>
#include "../src/model/Helper.h"
#include "../src/model/event/BaseEvent.h"
#include "../src/model/event/SingleEvent.h"
#include "../src/model/event/RecurringEvent.h"

int main() {
    SingleEvent single1;
    single1.name = "sleep";
    single1.note = "note";
    single1.location = "Prague";
    Date start("26.6.2023 23:59"), end("27.6.2023 08:00");
    single1.setTags("tag1, tag2, tag3");
    single1.setParticipants("participant1, participant2, participant3");
    single1.setDate(start, end);
    std::string single1Print = single1.print();
    std::string single1Target = "SingleEvent\n"
                                "sleep\n"
                                "note\n"
                                "Prague\n"
                                "26.06.2023 23:59\n"
                                "27.06.2023 08:00\n"
                                "tag1,tag2,tag3\n"
                                "participant1,participant2,participant3\n";
    assert(single1Print == single1Target);
    RecurringEvent rec1;
    rec1.name = "sleep";
    rec1.note = "note";
    rec1.location = "Prague";
    rec1.setTags("tag1, tag2, tag3");
    rec1.setParticipants("participant1, participant2, participant3");
    rec1.setDate(start, end);
    rec1.setRepeatIntervalDays(6);
    rec1.setRepeatCount(100);
    std::string rec1print = rec1.print();
    std::string rec1Target = "RecurringEvent\n"
                             "sleep\n"
                             "note\n"
                             "Prague\n"
                             "26.06.2023 23:59\n"
                             "27.06.2023 08:00\n"
                             "tag1,tag2,tag3\n"
                             "participant1,participant2,participant3\n" +
                             std::to_string(6 * 24 * 60 * 60) + "\n"
                                                                "100\n";
    assert(rec1print == rec1Target);
}