#pragma once

#include "BaseView.h"
#include "form.h"
#include "../wrapper/FormWrapper.h"

/**
 * View in which the user can edit, search, remove and add all types of events
 */
class EditView : public BaseView {
public:
    explicit EditView(std::shared_ptr<Calendar> calendar);

    ViewState handleKey(int key_pressed) override;

    void draw() override;
protected:
    /**
     * @brief This method is responsible for searching event by ID.
     */
    ViewState searchByID();

    /**
     * @brief This method is responsible adding single events
     */
    ViewState addSingleEvent();

    /**
     * @brief This method is responsible adding recurring events
     */
    ViewState addRecurringEvent();

    /**
     * @brief This method is responsible editing Single/Recurring events
     */
    ViewState editEvent();

    /**
     * @brief This method is responsible deleting Single/Recurring events
     */
    ViewState deleteEvent();

    /**
     * @brief This method is responsible searching events based on a prompt
     */
    ViewState search();

    std::shared_ptr<Calendar> m_calendar;
    const std::vector<std::string> options = {"Event ID [1, ...]", "Name", "Note", "Location", "Start", "End",
                                              "Tags (delimiter ',')", "Participants (delimiter ',')",
                                              "Repeat Interval in days [1, 365]", "Repeat Count [1, 100]",
                                              "Search"};
    FormWrapper formObject{options};
};
