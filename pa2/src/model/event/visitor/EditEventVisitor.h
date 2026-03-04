#pragma once

#include "EventVisitor.h"

/**
 * Visitor that edits events based on a form
 */
class EditEventVisitor : public EventVisitor {
public:
    EditEventVisitor() = default;

    /**
     * Constructor
     * @param args vector of attributes to be edited
     */
    explicit EditEventVisitor(const std::vector<std::string> &args): m_attributes(args) {}

    /**
     * @brief Sets attribute of SingleEvent
     * @param event
     */
    void visit(SingleEvent &event) override;

    /**
     * @brief Sets attribute of RecurringEvent
     * @param event
     */
    void visit(RecurringEvent &event) override;

    /**
     * @brief Sets attributes to be edited
     * @param args
     */
    void setAttributes(const std::vector<std::string> &args) { m_attributes = args; }
protected:
    std::vector<std::string> m_attributes;
};


