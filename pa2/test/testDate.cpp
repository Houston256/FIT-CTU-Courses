#include <cassert>
#include "../src/model/wrapper/Date.h"

int main() {
    Date date1("1.1.2021 12:30");
    Date date2(1, 1, 2021, 12, 30);
    assert(date1 == date2);
    date1.addDays(3);
    date1.addMonths(5);
    date1.addHours(-2);
    date1.addMinutes(-31);
    assert(date1 == Date("4.6.2021 09:59"));
    Date date3("1.1.2021 12:31");
    Date date4("1.1.2021 12:29");
    assert(Date("24.05.2023 23:25").toString() == "24.05.2023 23:25");
    assert(std::abs(difftime(date4.getTime(), date3.getTime())) == 2 * 60);

    Date foo("27.5.2023 18:12");
    assert(foo.toString() == "27.05.2023 18:12");
    assert(Date::getStartOfDay(foo).toString() == "27.05.2023 00:00");
    assert(Date::getEndOfDay(foo).toString() == "27.05.2023 23:59");
    assert(Date::getStartOfWeek(foo).toString() == "21.05.2023 00:00");
    assert(Date::getEndOfWeek(foo).toString() == "27.05.2023 23:59");
    assert(Date::getStartOfMonth(foo).toString() == "01.05.2023 00:00");
    assert(Date::getEndOfMonth(foo).toString() == "31.05.2023 23:59");
    assert(Date::getStartOfMonth(Date("01.01.2021 12:30")).toString() == "01.01.2021 00:00");
    assert(Date::getEndOfMonth(Date("01.01.2021 12:30")).toString() == "31.01.2021 23:59");
    Date now, start, end;
    start = Date::getStartOfWeek(now);
    end = Date::getEndOfWeek(now);
    auto current_date_range = std::make_pair(start, end);
    for (int i = 0; i < 1000; ++i) {
        current_date_range.first.addDays(7);
        current_date_range.first = Date::getStartOfWeek(current_date_range.first);
        current_date_range.second = Date::getEndOfWeek(current_date_range.first);
        assert(current_date_range.first.getHour() == 0); // always fails in 10th month
        assert(current_date_range.first.getMinute() == 0);
        // this never fails
        assert(current_date_range.second.getHour() == 23);
        assert(current_date_range.second.getMinute() == 59);
    }
    return 0;
}