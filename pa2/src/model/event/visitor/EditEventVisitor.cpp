#include "EditEventVisitor.h"

void EditEventVisitor::visit(SingleEvent &event) {
    event.name = m_attributes[1];
    event.note = m_attributes[2];
    event.location = m_attributes[3];
    Date start(m_attributes[4]), end(m_attributes[5]);
    event.setDate(start, end);
    event.setTags(m_attributes[6]);
    event.setParticipants(m_attributes[7]);
}

void EditEventVisitor::visit(RecurringEvent &event) {
    Date start(m_attributes[4]), end(m_attributes[5]);
    event.name = m_attributes[1];
    event.note = m_attributes[2];
    event.location = m_attributes[3];
    event.setDate(start, end);
    event.setTags(m_attributes[6]);
    event.setParticipants(m_attributes[7]);
    // set repeat interval
    time_t repInterval;
    try {
        repInterval = static_cast<time_t>(stoul(m_attributes[8]));
    } catch (...) {
        throw std::runtime_error("Can't convert \"" + m_attributes[8] + "\" to repeat interval");
    }
    event.setRepeatIntervalDays(repInterval);
    // set repeat count
    size_t repCount;
    try {
        repCount = stoul(m_attributes[9]);
    }
    catch (...) {
        throw std::runtime_error("Can't convert \"" + m_attributes[9] + "\" to repeat count");
    }
    event.setRepeatCount(repCount);
}

