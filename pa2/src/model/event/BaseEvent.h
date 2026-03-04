#pragma once

#include <ctime>
#include <list>
#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include "../wrapper/Date.h"
#include "../Helper.h"
#include "../../view/wrapper/FormWrapper.h"
#include "../../view/wrapper/Popup.h"

class SingleEvent;

class RecurringEvent;

/**
 * @brief The BaseEvent class
 * @details The BaseEvent class is the base class of all events.
 */
class BaseEvent {
public:
    size_t id = 1;
    std::string name;
    std::string note;
    std::string location;

    //--------------------------------------------- Constructors--------------------------------------------------------

    BaseEvent() = default;

    explicit BaseEvent(std::istream &is);

    /**
     * @brief BaseEvent constructor
     * @param id
     * @param start
     * @param end
     * @param name
     * @param textNote
     * @throws std::invalid_argument if start >= end or if start or end are invalid
     */
    BaseEvent(size_t id, const Date &start, const Date &end, std::string name, std::string textNote);

    /**
     * @brief BaseEvent constructor
     * @param id
     * @param name
     * @param textNote
     * @param location
     * @param start
     * @param end
     * @param participants
     * @throws std::invalid_argument if (start >= end),  (start or end are invalid) or if participants can't be parsed
     */
    BaseEvent(size_t id, std::string name, std::string textNote, std::string location, const std::string &start,
              const std::string &end, const std::string &participants, const std::string &tags);

    /**
     * @brief BaseEvent destructor
     */
    virtual ~BaseEvent() = default;

    //--------------------------------------------- Operators ----------------------------------------------------------
    /**
     * @brief Sends the event to an output stream.
     * @param os output stream
     * @param event event to be printed
     * @return outputstream
     */
    friend std::ostream &operator<<(std::ostream &os, const BaseEvent &event) { return (os << event.print()); }

    /**
     * @brief Returns true if rhs is smaller than this event
     * @param rhs event to be compared
     */
    [[nodiscard]] bool operator<(const BaseEvent &rhs) const;

    [[nodiscard]] virtual inline bool operator<(const Date &rhs) const {
        return getStartDate() < rhs;
    }

    [[nodiscard]] virtual inline bool operator>(const Date &rhs) const {
        return getStartDate() > rhs;
    }

    //--------------------------------------------- Collision ----------------------------------------------------------

    /**
     * @brief Checks if this event collides with event given as parameter.
     * @param event to check the collision with
     * @return true if events collide
     */
    [[nodiscard]] virtual bool isCollision(const BaseEvent *event) const = 0;

    /**
     * @brief Checks if this event collides with event given as parameter.
     * @param event to check the collision with
     * @return true if events collide
     */
    [[nodiscard]] virtual bool isCollision(const SingleEvent *event) const = 0;

    /**
     * @brief Checks if this event collides with event given as parameter.
     * @param event to check the collision with
     * @return true if events collide
     */
    [[nodiscard]] virtual bool isCollision(const RecurringEvent *event) const = 0;

    /**
     * @brief Checks if event collides with a date interval.
     * @param start date of date interval
     * @param end of date interval
     * @return true if event collides with date interval
     */
    [[nodiscard]] virtual bool isCollision(const Date &start, const Date &end) const = 0;

    //--------------------------------------------- Getters ----------------------------------------------------------

    [[nodiscard]] inline time_t getDuration() const { return m_end - m_start; }

    [[nodiscard]] inline Date getStartDate() const { return m_start; }

    [[nodiscard]] inline Date getEndDate() const { return m_end; }

    [[nodiscard]] inline std::vector<std::string> getParticipants() const { return m_participants; }

    [[nodiscard]] inline std::vector<std::string> getTags() const { return m_tags; }

    std::string getTagsString() const;

    std::string getParticipantsString() const;

    BaseEvent &setDate(const Date &start, const Date &end);

    BaseEvent &setParticipants(const std::string &str, const std::string &delimiter = ",");

    BaseEvent &setTags(const std::string &str, const std::string &delimiter = ",");

    virtual std::shared_ptr<BaseEvent> clone() const = 0;

    //----------------------------------------- Output methods ---------------------------------------------------------

    /**
     * @brief Returns string representation of this event which is used when saving the event to a file.
     * @return string representation of this event
     */
    [[nodiscard]] virtual std::string print() const;

    /**
     * @brief Prints this event to the console.
     * @param form to which the event is printed
     */
    virtual void print(FormWrapper &form) const;

    /**
     * @brief Creates a popup window with this event.
     */
    [[nodiscard]] virtual Popup show() const = 0;

    /**
     * @brief Accepts visitor which will set attributes of this event.
     * @param v visitor to be accepted
     */
    virtual void accept(class EventVisitor &v) = 0;

    //-------------------------------------------- Evaluate ------------------------------------------------------------

    /**
     * @brief Evaluates expression and returns true if this event matches the expression.
     * @param expression a string containing an expression
     * @return true if this event matches the expression
     */
    bool evaluateExpression(const std::string &expression) const;

    /**
     * @brief Evaluates token and returns true if this event matches the token.
     * @param token in string representation
     * @return true if this event matches the token
     */
    bool evaluateToken(const std::string &token) const;

    //------------------------------------------- Helpers --------------------------------------------------------------
    /**
     * @brief Returns true if this event includes the date given as parameter.
     * @param date to be checked
     * @return true if this event includes the date given as parameter
     */
    [[nodiscard]] inline bool includes(const Date &date) const { return m_start <= date && date <= m_end; }

protected:
    Date m_start;
    Date m_end = Date(m_start).addHours(1);
    std::vector<std::string> m_tags;
    std::vector<std::string> m_participants;
};

