#include <sstream>
#include <cassert>
#include "BaseEvent.h"
#include "../Helper.h"


BaseEvent::BaseEvent(size_t id, const Date &start, const Date &end, std::string name, std::string textNote) :
        id(id),
        name(std::move(name)),
        note(std::move(textNote)) {
    setDate(start, end);
}

BaseEvent::BaseEvent(size_t id, std::string name, std::string textNote, std::string location, const std::string &start,
                     const std::string &end, const std::string &participants, const std::string &tags) :
        id(id),
        name(std::move(name)),
        note(std::move(textNote)),
        location(std::move(location)) {
    setDate(Date(start), Date(end));
    setParticipants(participants);
    setTags(tags);
}

bool BaseEvent::operator<(const BaseEvent &rhs) const {
    return std::tie(m_start, m_end) < std::tie(rhs.m_start, rhs.m_end);
}

BaseEvent &BaseEvent::setDate(const Date &start, const Date &end) {
    if (start >= end) {
        throw std::invalid_argument("BaseEvent::setDate start must be after end.");
    }
    m_start = start;
    m_end = end;
    return *this;
}

BaseEvent &BaseEvent::setTags(const std::string &str, const std::string &delimiter) {
    // clear tags
    m_tags.clear();
    // split str into tokens
    auto tmp = Helper::split(str, delimiter);
    // add non-empty tokens to participants
    for (auto &tag: tmp) {
        tag = Helper::trim(tag);
        if (tag.empty()) { continue; }
        m_tags.emplace_back(tag);
    }
    return *this;
}

BaseEvent &BaseEvent::setParticipants(const std::string &str, const std::string &delimiter) {
    // clear participants
    m_participants.clear();
    // split str into tokens
    auto tmp = Helper::split(str, delimiter);
    // add non-empty tokens to participants
    for (auto &person: tmp) {
        person = Helper::trim(person);
        if (person.empty()) { continue; }
        m_participants.emplace_back(person);
    }
    return *this;
}

std::string BaseEvent::print() const {
    std::string res;
    res += name + "\n";
    res += note + "\n";
    res += location + "\n";
    res += m_start.toString() + "\n";
    res += m_end.toString() + "\n";
    res += Helper::join(m_tags.begin(), m_tags.end(), ",") + "\n";
    res += Helper::join(m_participants.begin(), m_participants.end(), ",") + "\n";
    return res;
}

void BaseEvent::print(FormWrapper &form) const {
    assert(form.getNumberOfFields() >= 7);
    form.setFieldValue(1, name);
    form.setFieldValue(2, note);
    form.setFieldValue(3, location);
    form.setFieldValue(4, m_start.toString());
    form.setFieldValue(5, m_end.toString());
    form.setFieldValue(6, getTagsString());
    form.setFieldValue(7, getParticipantsString());
}

std::string BaseEvent::getParticipantsString() const {
    return Helper::join(m_participants.begin(), m_participants.end(), ",");
}

std::string BaseEvent::getTagsString() const {
    return Helper::join(m_tags.begin(), m_tags.end(), ",");
}

BaseEvent::BaseEvent(std::istream &is) {
    if (!is) {
        throw std::invalid_argument("BaseEvent::BaseEvent input stream is not valid.");
    }
    std::string line;
    size_t counter = 0;
    while (counter <= 6 && std::getline(is, line)) {
        line = Helper::trim(line);
        switch (counter) {
            case 0:
                name = line;
                break;
            case 1:
                note = line;
                break;
            case 2:
                location = line;
                break;
            case 3:
                m_start = Date(line);
                break;
            case 4:
                m_end = Date(line);
                break;
            case 5:
                setDate(m_start, m_end);
                setTags(line);
                break;
            case 6:
                setParticipants(line);
                break;
            default:
                throw std::invalid_argument("BaseEvent::BaseEvent too many lines in input stream.");
        }
        counter++;
    }
    if (counter != 7) {
        throw std::invalid_argument("BaseEvent::BaseEvent too few lines in input stream.");
    }
}

bool BaseEvent::evaluateExpression(const std::string &expression) const {
    std::string left, right;
    auto andPos = expression.find("AND");

    // AND found
    if (andPos != std::string::npos) {
        left = expression.substr(0, andPos);
        right = expression.substr(andPos + 3);
        return evaluateExpression(left) && evaluateExpression(right);
    } else { // AND not found, check for OR
        auto orPos = expression.find("OR");
        // OR found
        if (orPos != std::string::npos) {
            left = expression.substr(0, orPos);
            right = expression.substr(orPos + 2);
            return evaluateExpression(left) || evaluateExpression(right);
        }
            // OR not found, evaluate single token
        else {
            return evaluateToken(expression);
        }
    }
}


bool BaseEvent::evaluateToken(const std::string &token) const {
    // UNTIL 12.12.2000 12:00
    // AFTER 12.12.2000 12:00
    // NAME name
    // NOTE note
    // LOCATION prague
    // TAG tag
    // CONTAINS participant
    auto tokenTrimmed = Helper::trim(token);
    auto words = Helper::split(tokenTrimmed, " ");
    if (words.empty())
        throw (std::invalid_argument("Invalid token: " + token));

    if (words[0] == "UNTIL")
        return *this < Date(tokenTrimmed.substr(5, tokenTrimmed.size()));
    if (words[0] == "AFTER")
        return *this > Date(tokenTrimmed.substr(5, tokenTrimmed.size()));
    if (words[0] == "NAME")
        return name == Helper::trim(tokenTrimmed.substr(4, tokenTrimmed.size()));
    if (words[0] == "NOTE")
        return note == Helper::trim(tokenTrimmed.substr(4, tokenTrimmed.size()));
    if (words[0] == "LOCATION")
        return location == Helper::trim(tokenTrimmed.substr(8, tokenTrimmed.size()));
    if (words[0] == "TAG")
        return Helper::contains(getTags(), Helper::trim(tokenTrimmed.substr(3, tokenTrimmed.size())));
    if (words[0] == "CONTAINS")
        return Helper::contains(getParticipants(), Helper::trim(tokenTrimmed.substr(8, tokenTrimmed.size())));

    throw std::invalid_argument("Invalid token: " + token);
}









