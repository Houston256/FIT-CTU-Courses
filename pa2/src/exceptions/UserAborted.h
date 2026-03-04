#pragma once

#include <exception>
#include <string>

/**
 * Exception thrown when user aborts program
 */
class UserAborted : public std::exception {
public:
    [[nodiscard]] const char *what() const noexcept override { return "User aborted program"; }
};