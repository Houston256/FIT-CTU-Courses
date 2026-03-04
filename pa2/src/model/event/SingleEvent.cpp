#include "SingleEvent.h"
#include "RecurringEvent.h"

bool SingleEvent::isCollision(const BaseEvent *event) const {
    if (event == this) { return true; }
    return event->isCollision(this);
}

bool SingleEvent::isCollision(const SingleEvent *event) const {
    if (event == this) { return true; }
    return isCollision(event->m_start, event->m_end);
}

bool SingleEvent::isCollision(const RecurringEvent *event) const {
    // could be handles here, but it would be a duplicate
    // of the implementation in RecurringEvent
    return event->isCollision(this);
}

bool SingleEvent::isCollision(const Date &other_start, const Date &other_end) const {
    SingleEvent other;
    other.setDate(other_start, other_end);
    bool res = (includes(other_start) && other_start != m_end) // other_start in [m_start, m_end)
               || (includes(other_end) && other_end != m_start)// other_end in (m_start, m_end]
               || (other.includes(m_start) && m_start != other.m_end) // other_start in [m_start, m_end)
               || (other.includes(m_end) && m_end != other.m_start); // other_end in (m_start, m_end]
    return res;
}

std::string SingleEvent::print() const {
    std::string res = "SingleEvent\n";
    return res += BaseEvent::print();
}

void SingleEvent::accept(class EventVisitor &v) {
    v.visit(*this);
}

std::shared_ptr<BaseEvent> SingleEvent::clone() const {
    return std::make_shared<SingleEvent>(*this);
}

Popup SingleEvent::show() const {
    std::vector<std::string> lines;
    lines.emplace_back("Name: " + name);
    lines.emplace_back("Note: " + note);
    lines.emplace_back("Location: " + location);
    lines.emplace_back("Start: " + m_start.toString());
    lines.emplace_back("End: " + m_end.toString());
    lines.emplace_back("Tags: " + getTagsString());
    lines.emplace_back("Participants: " + getParticipantsString());
    return Popup(lines, "Single event details");
}

