#pragma once

#include <cmath>
#include "BaseEvent.h"
#include "visitor/EventVisitor.h"
#include "../Constants.h"

/**
 * @Brief Represents a recurring event.
 * Inherits from BaseEvent.
 * @details Contains additional variables which hold number of repetitions and interval between repetitions.
 */
class RecurringEvent : public BaseEvent {
protected:
    time_t m_repeatInterval = 1; // repeat interval in seconds
    size_t m_repeatCount = 1;

public:

    RecurringEvent() = default;

    /**
     * @brief Constructor RecurringEvent from istream
     * @param is stream from which to construct
     */
    explicit RecurringEvent(std::istream &is) : BaseEvent(is) {
        is >> m_repeatInterval >> m_repeatCount;
        setRepeatIntervalSeconds(m_repeatInterval);
        setRepeatCount(m_repeatCount);
    }

    /**
     * @Brief Constructor.
     * @param id
     * @param name
     * @param description
     * @param location
     * @param start_date
     * @param end_date
     * @param participants
     * @param repeat_interval in days
     * @param repeat_count
     */
    RecurringEvent(size_t id, std::string name, std::string description, std::string location,
                   const std::string &start_date, const std::string &end_date, const std::string &participants,
                   const std::string &tags, time_t repeat_interval, size_t repeat_count);

    /**
     * @brief Check if this event collides with another event of unknown class.
     * @param event to check
     * @return true if collision, false otherwise
     */
    bool isCollision(const BaseEvent *event) const override;

    /**
     * @brief Check if this event collides with SingleEvent class.
     * @param event to check
     * @return true if collision, false otherwise
     */
    bool isCollision(const SingleEvent *event) const override;

    /**
     * @brief Check if this event collides with another date of same type.
     * @param event to check
     * @return true if collision, false otherwise
     */
    bool isCollision(const RecurringEvent *event) const override;

    /**
     * @brief Check if this event collides with an interval
     * @param start_other start date of interval
     * @param end_other end date of interval
     * @return true if collision, false otherwise
     */
    [[nodiscard]] bool isCollision(const Date &start_other, const Date &end_other) const override;

    [[nodiscard]] std::string print() const override;

    /**
     * @brief Set new repeat interval.
     * @param other_interval
     * @throws std::invalid_argument if other_interval is not in range [1, 365]
     */
    void setRepeatIntervalDays(time_t other_interval);


    void setRepeatIntervalSeconds(time_t other_interval);

    /**
     * @brief Set new repeat count.
     * @param count in days
     * @throws std::invalid_argument if count is less than 1
     */
    void setRepeatCount(size_t count);

    inline time_t getIntervalSeconds() const { return m_repeatInterval; }

    inline time_t getIntervalDays() const {
        return static_cast<time_t>(m_repeatInterval / secondsInDay);
    }

    size_t getRepeatCount() const;

    void accept(class EventVisitor &v) override;

    inline std::pair<Date, Date> getLastOccurrence() const {
        return std::make_pair(m_start + m_repeatInterval * static_cast<time_t>(m_repeatCount),
                              m_end + m_repeatInterval * static_cast<time_t>(m_repeatCount));

    }

    inline bool operator<(const Date &rhs) const override {
        return getStartDate() < rhs;
    }

    inline bool operator>(const Date &rhs) const override {
        auto [lastStart, lastEnd] = getLastOccurrence();
        return lastStart > rhs;
    }

    std::shared_ptr<BaseEvent> clone() const override;

    void print(FormWrapper &form) const override;

    Popup show() const override;
};

