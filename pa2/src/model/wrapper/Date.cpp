#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <ctime>
#include "Date.h"

Date::Date(int d, int m, int y, int h, int min) {
    setTz();
    // Initialize m_date
    m_date.tm_mday = d;
    m_date.tm_mon = m - 1;
    m_date.tm_year = y - 1900;
    m_date.tm_hour = h;
    m_date.tm_min = min;
    m_date.tm_isdst = 0;
    // Check that m_date is valid
    if (!tmChangeOk()) {
        throw std::invalid_argument("Invalid date: "
                                    + std::to_string(d)
                                    + "." + std::to_string(m)
                                    + "." + std::to_string(y)
                                    + " " + std::to_string(h)
                                    + ":" + std::to_string(min));
    }
}

Date::Date(time_t time) {
    std::string utc_tz = "UTC";
    setenv("TZ", utc_tz.c_str(), 1);
    tzset();
    setDateTime(time);
    m_date.tm_isdst = 0;
}

/**
 * @brief Date::Date constructor which takes a string and a format string
 * @param date string containing the date
 * @param format format of date
 * @details All whitespaces from the date string are ignored.
 * @throw std::invalid_argument if date is invalid
 * @see strptime
 * @see tmChangeOk
 */
Date::Date(const std::string &date, const std::string &format) {
    setTz();
    // delete whitespaces
    m_date.tm_isdst = 0;
    // check if date is valid
    bool time_ok = strptime(date.c_str(), format.c_str(), &m_date) && tmChangeOk();
    if (!time_ok) {
        throw std::invalid_argument("Invalid date: " + date);
    }
}

bool Date::tmChangeOk() {
    m_time = mktime(&m_date);
    return (m_time != -1);
}

bool Date::timeChangeOk() {
    auto date_ptr = localtime(&m_time);
    if (!date_ptr) { return false; }
    m_date = *date_ptr;
    return true;
}

time_t Date::operator-(const Date &rhs) const {
    if (m_time < rhs.m_time) {
        throw std::invalid_argument("Date::operator-: rhs is greater than lhs");
    }
    return m_time - rhs.m_time;
}

Date &Date::operator-=(const Date &rhs) {
    if (m_time < rhs.m_time) {
        throw std::invalid_argument("Date::operator-=: rhs is greater than lhs");
    }
    m_time -= rhs.m_time;
    if (!timeChangeOk()) {
        throw std::invalid_argument("Invalid time_t");
    }
    return *this;
}

Date &Date::operator+=(const time_t &rhs) {
    if (rhs < 0) {
        throw std::invalid_argument("Date::operator+=: rhs is negative");
    }
    if (m_time >= time_t(std::numeric_limits<time_t>::max()) - rhs) {
        throw std::invalid_argument("Date::operator+=: time_t overflow");
    }
    return setDateTime(m_time + rhs);
}

Date &Date::addMonths(int months) {
    m_date.tm_mon += months;
    if (!tmChangeOk()) {
        throw std::invalid_argument("Invalid date");
    }
    return *this;
}

Date &Date::addYears(int years) {
    m_date.tm_year += years;
    if (!tmChangeOk()) {
        throw std::invalid_argument("Invalid date");
    }
    return *this;
}

std::string Date::toString(const std::string &format) const {
    std::ostringstream oss;
    oss << std::put_time(&m_date, format.c_str());
    return oss.str();
}

Date & Date::setDateTime(time_t rhs) {
    m_time = rhs;
    if (!timeChangeOk()) {
        throw std::invalid_argument("Invalid time_t");
    }
    return *this;
}

Date & Date::setDateTime(const tm &rhs) {
    m_date = rhs;
    if (!tmChangeOk()) {
        throw std::invalid_argument("Invalid date");
    }
    return *this;
}

Date::Date() {
    setTz();
    setDateTime(time(nullptr));
    m_date.tm_isdst = 0;
}

time_t Date::getTime() const {
    return m_time;
}

Date &Date::setMinute(int minute) {
    m_date.tm_min = minute;
    if (!tmChangeOk()) {
        throw std::invalid_argument("Invalid date");
    }
    return *this;
}

Date &Date::setSecond(int i) {
    m_date.tm_sec = i;
    if (!tmChangeOk()) {
        throw std::invalid_argument("Invalid date");
    }
    return *this;
}

Date &Date::setHour(int hour) {
    m_date.tm_hour = hour;
    if (!tmChangeOk()) {
        throw std::invalid_argument("Invalid date");
    }
    return *this;
}

Date &Date::setDay(int day) {
    m_date.tm_mday = day;
    if (!tmChangeOk()) {
        throw std::invalid_argument("Invalid date");
    }
    return *this;
}

Date &Date::setMonth(int month) {
    m_date.tm_mon = month - 1;
    if (!tmChangeOk()) {
        throw std::invalid_argument("Invalid date");
    }
    return *this;
}

Date &Date::setYear(int year) {
    m_date.tm_year = year - 1900;
    if (!tmChangeOk()) {
        throw std::invalid_argument("Invalid date");
    }
    return *this;
}

Date Date::getStartOfDay(const Date &date) {
    Date res{date};
    return res.setSecond(0).setMinute(0).setHour(0);
}

Date Date::getEndOfDay(const Date &date) {
    auto res = getStartOfDay(date);
    res.addDays(1).addSeconds(-1);
    return res;
}

Date Date::getStartOfWeek(const Date &date) {
    auto res = getStartOfDay(date);
    res.addDays(-res.getWday());
    return res;
}

Date Date::getEndOfWeek(const Date &date) {
    auto res = getStartOfWeek(date);
    res.addDays(7).addMinutes(-1);
    return res;
}

Date Date::getStartOfMonth(const Date &date) {
    Date res = getStartOfDay(date);
    res.setDay(1);
    return res;
}

Date Date::getEndOfMonth(const Date &date) {
    auto res = getStartOfMonth(date);
    res.addMonths(1).addMinutes(-1);
    return res;
}

Date &Date::setTz(const std::string &tz) {
    setenv("TZ", tz.c_str(), 1);
    tzset();
    return *this;
}

