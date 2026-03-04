#pragma once

#include "BaseView.h"
#include "../../model/Parser.h"

/**
 * Import, export files, clear calendar
 */
class IOView : public BaseView {
protected:
    std::shared_ptr<Calendar> m_calendar;
    // parser which is used for all IO operations
    Parser m_parser;
    // create a form with two options
    const std::vector<std::string> options = {"Filename", "Search"};
    FormWrapper formObj{options};
public:
    /**
     * @brief Constructor
     * @param calendar
     * @param parser which is used for all IO operations
     */
    IOView(const std::shared_ptr<Calendar> &calendar, const Parser &parser);

    ViewState handleKey(int key_pressed) override;

    void draw() override {
        curs_set(1);
        refresh();
    };

    /**
     * @brief This method is responsible for saving the calendar to a file
     */
    ViewState save();

    /**
     * @brief This method is responsible for loading the calendar from a file
     */
    ViewState load();

    /**
     * @brief This method is responsible for saving events that match a prompt to a file
     */
    ViewState saveSearch();
};

