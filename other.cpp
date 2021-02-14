#include "core.h"

std::ostream& operator<< (std::ostream& out, string_t& obj) {
    out << obj.c_str();
    return out;
}

std::istream& operator>> (std::istream& in, string_t& obj) {
    while (!in.eof()) {
        char ch_detector = 0;
        char str[2] = { ch_detector, 0 };
        in >> std::noskipws >> ch_detector;
        str[0] = ch_detector;
        obj.append(str);
    }
    return in;
}

AdapterPDCurses::AdapterPDCurses() {
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    attron(COLOR_PAIR(1));
}

void AdapterPDCurses::set_title(const string_t& title) {
    PDC_set_title(title.c_str());
}

int AdapterPDCurses::set_string(unsigned short y, unsigned short x, const string_t& text) {
    return mvaddstr(y, x, text.c_str());
}

int AdapterPDCurses::set_string(unsigned short y, unsigned short x, char text) {
    move(y, x);
    return addch(text);
}

void AdapterPDCurses::get_maxxy(int* x, int* y) {
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    *y = info.srWindow.Bottom - info.srWindow.Top + 1, * x = info.dwMaximumWindowSize.X;
    return;
}

bool AdapterPDCurses::is_resize(int prev_x, int prev_y) {
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    int del = info.srWindow.Bottom - info.srWindow.Top + 1;
    int x = info.dwMaximumWindowSize.X; int y = info.dwMaximumWindowSize.Y;
    if (x != prev_x || y != prev_y || del != y) {
        resize_term(del, x);
        return true;
    }
    return false;
}

void AdapterPDCurses::partial_cleaning(int first_y, int num_row) {
    int x, y;
    get_maxxy(&x, &y);
    string_t blank(x, ' ');
    for (unsigned int y = first_y; y < first_y + num_row; y++) {
        set_string(y, 0, blank);
    }
}