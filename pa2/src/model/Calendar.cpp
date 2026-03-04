#include <algorithm>
#include "Calendar.h"

void Calendar::addEvent(const std::shared_ptr<BaseEvent> &event) {
    // iterate over whole vector and check if there is a collision
    for (auto &e: m_events) {
        if (e->isCollision(event.get())) {
            throw std::runtime_error("Collision with [" + std::to_string(e->id) + "] detected");
        }
    }
    event->id = getAvailableId();
    m_events.emplace_back(event);
}

bool Calendar::removeEvent(size_t event_id) {
    auto remove_it = std::remove_if(m_events.begin(), m_events.end(),
                                    [event_id](const std::shared_ptr<BaseEvent> &e) {
                                        return e->id == event_id;
                                    });
    bool removed = remove_it != m_events.end();
    m_events.erase(remove_it, m_events.end());
    return removed;
}

[[nodiscard]] std::shared_ptr<BaseEvent> Calendar::searchEvents(size_t id) const {
    for (const auto &event: m_events) {
        if (event->id == id) {
            return event;
        }
    }
    return nullptr;
}

[[nodiscard]] size_t Calendar::getAvailableId() {
    return availableId++;
}

[[nodiscard]] std::vector<std::shared_ptr<BaseEvent>> Calendar::searchEvents(const Date &start, const Date &end) const {
    std::vector<std::shared_ptr<BaseEvent>> result;
    for (auto &event: m_events) {
        if (event->isCollision(start, end)) {
            result.emplace_back(event);
        }
    }
    return result;
}

[[nodiscard]] std::vector<std::shared_ptr<BaseEvent>> Calendar::getEvents() const {
    return m_events;
}

std::vector<std::shared_ptr<BaseEvent>> Calendar::searchEvents(const std::string &prompt) const {
    std::vector<std::shared_ptr<BaseEvent>> res;
    for (const auto &event: m_events) {
        if (event->evaluateExpression(prompt)) {
            res.emplace_back(event);
        }
    }
    return res;
}

void Calendar::addEvent(const std::shared_ptr<BaseEvent> &event, size_t id) {
    // iterate over whole vector and check if there is a collision
    for (auto &e: m_events) {
        if (e->isCollision(event.get())) {
            throw std::runtime_error("Collision with [" + std::to_string(e->id) + "] detected");
        }
    }
    event->id = id;
    m_events.emplace_back(event);
}

void Calendar::clear() {
    m_events.clear();
    availableId = 1;
}

Calendar::Calendar(const std::vector<std::shared_ptr<BaseEvent>> &events) {
    addEvent(events);
}

void Calendar::addEvent(const std::vector<std::shared_ptr<BaseEvent>> &events) {
    for (const auto &event: events) {
        addEvent(event);
    }
}
