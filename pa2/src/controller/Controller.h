#pragma once

#include <memory>
#include "../view/application_view/BaseView.h"
#include "../view/application_view/MainMenuView.h"
#include "../model/Parser.h"

/**
 * @brief The Controller class
 * @details The Controller class is the main class of the application.
 * It is responsible for the communication between the model and the view.
 */
class Controller {
private:
    // Calendar which is passed to other classes, so they can access the calendar.
    std::shared_ptr<Calendar> calendar_ptr;
    // Current view of the application. (at first it is main menu, can change based on user input)
    std::shared_ptr<BaseView> curr_view_ptr;
    std::filesystem::path m_save_path;
    Parser m_parser;
public:
    Controller() = delete;

    explicit Controller(const std::string &savePath);

    ~Controller() { endwin(); }

    /**
     * @brief Start the application.
     * @details Runs in an infinite loop, getting user input and calling the appropriate methods.
     * @return 0 if the application ended successfully, 1 otherwise.
     */
    int run();

    /**
     * @brief Handle latest user input.
     * @param key_pressed latest user input
     * @details Keys that are not handled by the application are passed to the current view.
     * Keys like 'esc' or 'ctrl+d' are handled right away and don't need to be passed to the view.
     */
    void handleInput(int key_pressed);

    /**
     * @brief Setup the screen.
     */
    static void setup_scr();
};