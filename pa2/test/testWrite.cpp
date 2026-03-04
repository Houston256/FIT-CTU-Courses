#include "../src/model/Parser.h"
#include <cassert>
#include <filesystem>
#include <iostream>

int main() {
    Parser parser("examples");
    Calendar calendar;
    auto test_event = std::make_shared<SingleEvent>(SingleEvent{
            1,
            "name",
            "note",
            "location",
            "10.6.2022 12:30",
            "19.6.2022 13:30",
            "1, 2, 3, 4, dsjgdf, Jack, Jeff",
            "tag1, tag2, tag3"
    });

    auto test_event2 = std::make_shared<SingleEvent>(SingleEvent{
            1,
            "Some other name",
            "wooohoooooooo",
            "Wooow",
            "19.6.2022 13:30",
            "19.6.2022 14:00",
            "1, 2, 3, 4, dsjgdf, Jack, Jeff",
            "tag1, tag2, tag3"
    });
    try {
        calendar.addEvent(test_event);
        calendar.addEvent(test_event2);
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        assert(false);
    }
    try {
        parser.write("test.cal", std::make_shared<Calendar>(calendar));
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        std::filesystem::remove(std::filesystem::current_path() / "examples" / "test.cal");
        assert(false);
    }
    auto idk = parser.read("test.cal");
    std::filesystem::remove(std::filesystem::current_path() / "examples" / "test.cal");
    assert(idk.size() == 2);
    assert(idk[0]->name == "name");
    assert(idk[1]->name == "Some other name");
    return 0;
}