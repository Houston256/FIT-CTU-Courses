#include "RecurringEvent.h"

#include <utility>
#include "SingleEvent.h"

bool RecurringEvent::isCollision(const BaseEvent *event) const {
    return event->isCollision(this);
}

bool RecurringEvent::isCollision(const SingleEvent *event) const {
    return isCollision(event->getStartDate(), event->getEndDate());
}

bool RecurringEvent::isCollision(const RecurringEvent *event) const {
    if (event == this) { return true; }
    auto start_copy = m_start;
    auto end_copy = m_end;

    for (size_t i = 0; i < m_repeatCount + 1; i++) {
        if (event->isCollision(start_copy, end_copy)) {
            return true;
        }
        start_copy += m_repeatInterval;
        end_copy += m_repeatInterval;
    }
    return false;
}

bool RecurringEvent::isCollision(const Date &start_other, const Date &end_other) const {
    Date start_copy(m_start);
    Date end_copy(m_end);
    SingleEvent shifted;
    for (size_t i = 0; i < m_repeatCount + 1; i++) {
        shifted.setDate(start_copy, end_copy);
        if (shifted.isCollision(start_other, end_other)) { return true; }
        start_copy += m_repeatInterval;
        end_copy += m_repeatInterval;
    }
    return false;
}

std::string RecurringEvent::print() const {
    std::string res = "RecurringEvent\n";
    res += BaseEvent::print();
    res += std::to_string(m_repeatInterval) + "\n";
    res += std::to_string(m_repeatCount) + "\n";
    return res;
}

void RecurringEvent::setRepeatIntervalSeconds(time_t other_interval) {
    if (other_interval < 1 || (other_interval / secondsInDay) > 365) {
        throw std::invalid_argument(
                "RecurringEvent::setRepeatIntervalDays: other_interval must be in range [1...365] days");
    }
    this->m_repeatInterval = other_interval;
}

void RecurringEvent::setRepeatIntervalDays(time_t other_interval) {
    if (other_interval < 1 || other_interval > 365) {
        throw std::invalid_argument("RecurringEvent::setRepeatIntervalDays: interval must be in range [1, 365] days");
    }
    this->m_repeatInterval = other_interval * static_cast<time_t>(secondsInDay);
}

void RecurringEvent::setRepeatCount(size_t count) {
    if (count < 1 || count > 100) {
        throw std::invalid_argument("RecurringEvent::setRepeatCount: count must be in range [1, 100]");
    }
    this->m_repeatCount = count;
}

RecurringEvent::RecurringEvent(size_t id, std::string name, std::string description, std::string location,
                               const std::string &start_date, const std::string &end_date,
                               const std::string &participants,
                               const std::string &tags, time_t repeat_interval, size_t repeat_count) :
        BaseEvent(id,
                  std::move(name),
                  std::move(description),
                  std::move(location),
                  start_date,
                  end_date,
                  participants,
                  tags) {
    setRepeatIntervalDays(repeat_interval);
    setRepeatCount(repeat_count);
}


size_t RecurringEvent::getRepeatCount() const {
    return m_repeatCount;
}

void RecurringEvent::accept(class EventVisitor &v) {
    v.visit(*this);
}

std::shared_ptr<BaseEvent> RecurringEvent::clone() const {
    return std::make_shared<RecurringEvent>(*this);
}


void RecurringEvent::print(FormWrapper &form) const {
    BaseEvent::print(form);
    form.setFieldValue(8, std::to_string(getIntervalDays()));
    form.setFieldValue(9, std::to_string(getRepeatCount()));
}

Popup RecurringEvent::show() const {
    std::vector<std::string> lines;
    lines.emplace_back("Name: " + name);
    lines.emplace_back("Note: " + note);
    lines.emplace_back("Location: " + location);
    lines.emplace_back("Start of first occurrence: " + m_start.toString());
    lines.emplace_back("End of first occurrence: " + m_end.toString());
    lines.emplace_back("Tags: " + getTagsString());
    lines.emplace_back("Participants: " + getParticipantsString());
    lines.emplace_back("Interval: " + std::to_string(getIntervalDays()) + " days");
    lines.emplace_back("Repeat count: " + std::to_string(getRepeatCount()));
    lines.emplace_back("Last occurrence: " + getLastOccurrence().first.toString() + " - " +
                       getLastOccurrence().second.toString());
    return Popup(lines, "Recurring event details");
}
