#pragma once
// ncurses key codes
#define ENTER_COMPLETE KEY_ENTER: case 10: case 13
// escape key
constexpr int ESC = 27;
// ctrl + d key combination
constexpr int CTRL_D = 4;

static constexpr size_t secondsInDay = 60 * 60 * 24;

constexpr static const char *week_days[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

constexpr static const char *months[12] = {"January", "February", "March", "April", "May", "June",
                                           "July", "August", "September", "October", "November", "December"};

constexpr static const char *daysShort[7]{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
