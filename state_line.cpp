#include "core.h"

ViewStateLine::ViewStateLine(
    unsigned int cur_x, unsigned int cur_y, 
    unsigned int width, unsigned int high, unsigned int cur_shift) {
    x = cur_x;
    y = cur_y;

    max_x = width;
    max_y = high;

    shift = cur_shift;

    mode_len = 0;
    status_len = 0;
}

void ViewStateLine::update_position(unsigned int new_x, unsigned int new_y) {
    x = new_x;
    y = new_y;
    display();
}

void ViewStateLine::update_status(const string_t& new_status) {
    state_line = new_status;
    display();
}

string_t& ViewStateLine::get_state_line() {
    return state_line;
    display();
}

void ViewStateLine::update_mode(const string_t& cur_mode) {
    name_mode = cur_mode;
    display();
}

void ViewStateLine::update_dimensions(unsigned int x, unsigned y) {
    max_x = x;
    max_y = y;
    display();
}

void ViewStateLine::display() {
    status_len = 0;
    mode_len = 0;

    unsigned short cur_y = 0;
    unsigned short cur_x = 0;

    partial_cleaning(0, shift);

    for (cur_x; cur_x < max_x; cur_x++)
        set_string(shift, cur_x, '-');
    cur_x = 0;

    string_t str_mode = "MODE: " + name_mode;
    if (max_x > str_mode.length()) {
        mode_len = str_mode.length();
        set_string(cur_y, cur_x, str_mode);
    }

    char value[100] = { 0 };
    sprintf(value, "row:%d,col:%d", x, y + 1);
    string_t value_position(value);
    cur_x = max_x - 1 - value_position.length();
    if (cur_x > mode_len) {
        set_string(cur_y, cur_x, value_position);
    }

    cur_y+=2; cur_x = 0;
    string_t str_status = "STATUS: " + state_line;
    if (max_x > cur_x + str_status.length()) {
        status_len = str_status.length();
        set_string(cur_y, cur_x, str_status);
    }
}