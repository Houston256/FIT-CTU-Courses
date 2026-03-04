#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include "ncurses.h"
#include "form.h"
#include "menu.h"
#include "ViewState.h"
#include "../../model/event/BaseEvent.h"
#include "../../model/Calendar.h"
#include "../../model/Constants.h"


/**
 * @brief BaseView class
 * @details This class is responsible for displaying the calendar and reacting to user input.
 */
class BaseView {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~BaseView() = default;

    /**
     * @brief This method is responsible for displaying the calendar and reacting to user input.
     * @param key_pressed key which was pressed
     */
    virtual ViewState handleKey(int key_pressed) = 0; //returns next State

    /**
     * @brief update view
     */
    virtual void draw() = 0;
};