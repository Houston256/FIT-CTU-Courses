#pragma once

#include <ctime>
#include <string>
#include <iostream>

/**
 * @brief A wrapper class for the C++ ctime library.
 */
class Date {
protected:
    // m_date and m_time both represent the same date and time,
    // but almost all operations work with both tm and time_t, so
    // it's more efficient to just store both.
    tm m_date{};     // valid tm structure
    time_t m_time{}; // seconds since epoch

    /**
     * @brief Checks if the last change to m_date was valid.
     * @return True if the date is valid, false otherwise.
     * @see mktime()
     * @see Date::equal()
     */
    [[nodiscard]] bool tmChangeOk();

    /**
    * @brief Checks if the last change to m_time was valid.
    * @return True if the date is valid, false otherwise.
     * @see localtime()
    */
    [[nodiscard]] bool timeChangeOk();

    /**
     * @brief Sets datetime based on specified time_t
     * @param rhs given value based on which time should be set
     * @throws std::invalid_argument if given invalid value;
     */
    Date & setDateTime(time_t rhs);

    /**
     * @brief Sets datetime based on specified tm
     * @param rhs given value based on which time should be set
     * @throws std::invalid_argument if given invalid value;
     */
    Date & setDateTime(const tm &rhs);

    /**
     * @brief Sets timezone based on specified string
     * @param tz name of timezone
     * @return Date & reference to this
     */
    Date &setTz(const std::string &tz = "UTC");

public:

    /**
     * @brief Constructs a Date object with the current date and time.
     * @throws std::runtime_error if the current date is invalid
     */
    Date();

    /**
     * @brief Construct a new Date object
     * @param d day of month
     * @param m month of year
     * @param y year
     * @param h hour
     * @param min minute
     * @throws std::invalid_argument if date is invalid
     */
    Date(int d, int m, int y, int h, int min);

    /**
     * @brief Construct a new Date object
     * @param date date as a string
     * @param format format of date string
     * @throws std::invalid_argument if date is invalid
     * @see strptime
     * @see timeChangeOk
     */
    explicit Date(const std::string &date, const std::string &format = "%d.%m.%Y %H:%M");

    /**
     * @brief Construct a new Date object
     * @param time time since epoch
     * @throws std::invalid_argument if date is invalid
     */
    explicit Date(time_t time);

    //---------------------------------------- Operators ----------------------------------------------------------------

    [[nodiscard]] inline bool operator==(const Date &rhs) const { return m_time == rhs.m_time; }

    [[nodiscard]] inline bool operator!=(const Date &rhs) const { return m_time != rhs.m_time; }

    [[nodiscard]] inline bool operator<(const Date &rhs) const { return m_time < rhs.m_time; }

    [[nodiscard]] inline bool operator>(const Date &rhs) const { return m_time > rhs.m_time; }

    [[nodiscard]] inline bool operator<=(const Date &rhs) const { return m_time <= rhs.m_time; }

    [[nodiscard]] inline bool operator>=(const Date &rhs) const { return m_time >= rhs.m_time; }

    /**
     * @brief minus operator, subtracts seconds
     * @param rhs time to subtract in seconds
     * @return *this->m_time - rhs
     */
    [[nodiscard]] time_t operator-(const Date &rhs) const;

    /**
    * @brief minus operator, subtracts seconds
    * @param rhs time to subtract in seconds
    * @return *this->m_time - rhs
    */
    [[nodiscard]] Date operator+(const time_t &rhs) const { return Date(m_time + rhs); }

    /**
     * overloaded operator, returns difference between two dates
     * @param rhs other date
     * @return result of subtraction
     * @throws std::invalid_argument if rhs is greater than this or datetime is invalid after subtraction
     */
    Date &operator-=(const Date &rhs);

    /**
     * overloaded operator, adds rhs seconds to this
     * @param rhs num of seconds to add
     * @return result of addition
     * @throws std::invalid_argument if datetime is invalid after addition
     */
    Date &operator+=(const time_t &rhs);

    //-------------------------------------------- Addition ------------------------------------------------------------
    Date &addMinutes(int minutes) { return setMinute(m_date.tm_min + minutes); }

    Date &addSeconds(int seconds) { return setSecond(m_date.tm_sec + seconds); }

    Date &addHours(int hours) { return setHour(m_date.tm_hour + hours); }

    Date &addDays(int days) { return setDay(m_date.tm_mday + days); }

    Date &addMonths(int months);

    Date &addYears(int years);

    //---------------------------------------------- Getters -----------------------------------------------------------

    [[nodiscard]] inline int getMinute() const { return m_date.tm_min; }

    [[nodiscard]] inline int getHour() const { return m_date.tm_hour; }

    [[nodiscard]] inline int getWday() const { return m_date.tm_wday; }

    [[nodiscard]] inline int getDay() const { return m_date.tm_mday; }

    [[nodiscard]] inline int getMonth() const { return m_date.tm_mon + 1; }

    [[nodiscard]] inline int getYear() const { return m_date.tm_year + 1900; }

    [[nodiscard]] static Date getStartOfDay(const Date &date);

    [[nodiscard]] static Date getEndOfDay(const Date &date);

    [[nodiscard]] static Date getStartOfWeek(const Date &date);

    [[nodiscard]] static Date getEndOfWeek(const Date &date);

    [[nodiscard]] static Date getStartOfMonth(const Date &date);

    [[nodiscard]] static Date getEndOfMonth(const Date &date);

    /**
     * @brief Gets m_time
     * @return number of seconds since epoch
     */
    [[nodiscard]] time_t getTime() const;

    //---------------------------------------------- Setters -----------------------------------------------------------

    Date &setSecond(int i);

    Date &setMinute(int minute);

    Date &setHour(int hour);

    Date &setDay(int day);

    Date &setMonth(int month);

    Date &setYear(int year);

    //---------------------------------------- Output methods ----------------------------------------------------------

    /**
     * @brief Convert Date to std::string using the specified format
     * @param format desired format of date string
     * @returns std::string representation of date
     * @throws std::invalid_argument if format is invalid
     * @throws std::runtime_error if date is invalid
     * @see https://en.cppreference.com/w/cpp/io/manip/put_time
     */
    [[nodiscard]] std::string toString(const std::string &format = "%d.%m.%Y %H:%M") const;

    /**
     * @brief Send Date to output stream using the specified format
     * @param os output stream where the date will be sent
     * @param date Date object to be sent
     * @returns output stream
     */
    friend std::ostream &operator<<(std::ostream &os, const Date &date) { return os << date.toString(); }
};
