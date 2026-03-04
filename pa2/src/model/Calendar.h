#pragma once

#include "event/BaseEvent.h"
#include "event/RecurringEvent.h"
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

/**
 * @brief A calendar which contains events.
 * @details This class ties together all events and provides a way to search, add, remove and edit events.
 */
class Calendar {
private:
    // vector of all events, sorted by start date, then end date (might be useful for searching optimization)
    std::vector<std::shared_ptr<BaseEvent>> m_events{};
    size_t availableId = 1; // id that will be assigned to the next event
public:
    Calendar() = default;

    /**
     * @brief Constructor from vector of events.
     * @param events vector of events
     * @see addEvent
     */
    explicit Calendar(const std::vector<std::shared_ptr<BaseEvent>> &events);

    /**
     * @brief Add an event to the calendar.
     * @param event to add
     * @throws std::runtime_error if event collides with another event
     */
    void addEvent(const std::shared_ptr<BaseEvent> &event);

    /**
     * @brief Add an event to the calendar.
     * @param event
     * @param id
     * @details This method can be used when you want to assign a specific id to an event.
     * (assuming it's not already taken up)
     */
    void addEvent(const std::shared_ptr<BaseEvent> &event, size_t id);

    /**
     * @brief Add multiple events to the calendar.
     * @param events vector of events
     */
    void addEvent(const std::vector<std::shared_ptr<BaseEvent>> &events);

    /**
     * @brief Remove an event from calendar.
     * @param event_id id of event to remove
     * @return true if event was removed, false otherwise
     */
    bool removeEvent(size_t event_id);

    /**
     * @brief Search events by id
     * @param id id to search for
     * @return pointer to event if found, nullptr otherwise
     */
    [[nodiscard]] std::shared_ptr<BaseEvent> searchEvents(size_t id) const;

    /**
     * @brief Search events based on a datetime interval
     * @param start of interval
     * @param end of interal
     * @return vector of pointers to events which match the interval
     */
    [[nodiscard]] std::vector<std::shared_ptr<BaseEvent>> searchEvents(const Date &start, const Date &end) const;

    /**
     * @brief Search events based on a prompt
     * @param prompt string representing a search prompt
     * @return vector of events which match the prompt
     */
    [[nodiscard]] std::vector<std::shared_ptr<BaseEvent>> searchEvents(const std::string &prompt) const;

    /**
     * @return vector of pointers to all events
     */
    [[nodiscard]] std::vector<std::shared_ptr<BaseEvent>> getEvents() const;

    /**
     * @return available id
     * @detals id is incremented after its returned
     */
    [[nodiscard]] size_t getAvailableId();

    /**
     * @brief Clear all events, reset available id
     */
    void clear();
};
