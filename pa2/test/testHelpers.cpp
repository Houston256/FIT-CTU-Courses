#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include "../src/model/Helper.h"

int main() {
    std::string str = "      Hello there     ";
    assert(Helper::ltrim(str) == "Hello there     ");
    assert(Helper::rtrim(str) == "      Hello there");
    assert(Helper::trim(str) == "Hello there");
    assert(Helper::trim("").empty());
    assert(Helper::trim("                           ").empty());
    str = "This,string,has,commas";
    std::vector<std::string> vec = Helper::split(str, ",");
    std::vector<std::string> target = {"This", "string", "has", "commas"};
    for (const auto &x:target) {
        assert(Helper::contains(vec, x));
    }
    assert(vec.size() == 4);
    assert(vec == target);
    assert(Helper::split(",,,,", ",").size() == 5);
    assert(Helper::split("    ", " ").size() == 5);
    str = ",,,f,"; // empty strings are not removed
    vec = Helper::split(str, ",");
    assert(vec.size() == 5);
    std::vector<std::string> vec2 = Helper::split("", ",");
    assert(vec2.size() == 1);
    return 0;
}