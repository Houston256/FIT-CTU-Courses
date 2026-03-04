#pragma once

#include "../SingleEvent.h"
#include "../RecurringEvent.h"

/**
 * Class with the visitor architecture
 * Allows to easily add functionality without changing the target classes
 */
class EventVisitor {
public:
    /**
     * @brief Visits a single event
     * @param event to visit
     */
    virtual void visit(SingleEvent &event) = 0;

    /**
     * @brief Visits a recurring event
     * @param event to visit
     */
    virtual void visit(RecurringEvent &event) = 0;
};


