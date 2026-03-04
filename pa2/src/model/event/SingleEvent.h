#pragma once

#include "BaseEvent.h"

/**
 * An event that has only a single occurrence and can't repeat itself.
 */
class SingleEvent : public BaseEvent {
public:
    using BaseEvent::BaseEvent;

    bool isCollision(const BaseEvent *event) const override;

    bool isCollision(const SingleEvent *event) const override;

    bool isCollision(const RecurringEvent *event) const override;

    bool isCollision(const Date &other_start, const Date &other_end) const override;

    [[nodiscard]] std::string print() const override;

    void accept(class EventVisitor &v) override;

    std::shared_ptr<BaseEvent> clone() const override;

    Popup show() const override;
};