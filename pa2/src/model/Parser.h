#pragma once

#include <string>
#include "Calendar.h"
#include<filesystem>

/**
 * @brief Parser class
 * @details Responsible for parsing I/O files.
 */
class Parser {
protected:
    // path to file to be read/written
    std::string savesPath = std::filesystem::current_path();
public:
    Parser() = default;

    explicit Parser(const std::filesystem::path &savesPath) : savesPath(savesPath) {}

    /**
     * @brief Parse file and return Calendar object
     * @param path path to file
     * @return calendar
     * @throws std::runtime_error if file is invalid
     */
    std::vector<std::shared_ptr<BaseEvent>> read(const std::filesystem::path &path);

    /**
     * @brief Write calendar to file
     * @param path path of output file
     * @param calendar to by written to file
     * @return number of events written to file
     * @throws std::runtime_error if file is invalid
     * @see Calendar::getEvents()
     * @see Event::output()
     */
    size_t write(const std::filesystem::path &path, const std::shared_ptr<Calendar> &calendar);

    /**
     * @brief Write events to file
     * @param path of output file
     * @param events vector of events
     * @return number of written events
     */
    size_t write(const std::filesystem::path &path, const std::vector<std::shared_ptr<BaseEvent>> &events);
};
