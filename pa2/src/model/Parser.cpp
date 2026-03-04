#include <fstream>
#include "Parser.h"
#include "event/SingleEvent.h"


size_t Parser::write(const std::filesystem::path &path, const std::shared_ptr<Calendar> &calendar) {
    // open file and write all events to it
    std::ofstream file(savesPath / path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }
    size_t written_events = 0;
    for (const auto &event: calendar->getEvents()) {
        file << (*event);
        written_events++;
    }
    file.close();
    if (file.is_open()) {
        throw std::runtime_error("Could not close file");
    }
    return written_events;
}

std::vector<std::shared_ptr<BaseEvent>> Parser::read(const std::filesystem::path &path) {
    std::vector<std::shared_ptr<BaseEvent>> events;
    std::ifstream file(savesPath / path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }
    std::string line;
    // read line
    while (std::getline(file, line)) {
        // trim line
        line = Helper::trim(line);
        // skip empty lines
        if (line.empty()) {
            continue;
        }
        if (line == "SingleEvent") {
            auto event = SingleEvent(file);
            events.emplace_back(std::make_shared<SingleEvent>(event));
        } else if (line == "RecurringEvent") {
            auto event = RecurringEvent(file);
            events.emplace_back(std::make_shared<RecurringEvent>(event));
        } else {
            throw std::runtime_error("Invalid file format: " + line);
        }
    }
    return events;
}

size_t Parser::write(const std::filesystem::path &path, const std::vector<std::shared_ptr<BaseEvent>> &events) {
    // open file and write all events to it
    std::ofstream file(savesPath / path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }
    size_t written_events = 0;
    for (const auto &event: events) {
        file << (*event);
        written_events++;
    }
    file.close();
    if (file.is_open()) {
        throw std::runtime_error("Could not close file");
    }
    return written_events;
}
