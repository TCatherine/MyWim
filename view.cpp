#include "core.h"

View::View(Controller* cut_ctrl) {
    string_t title = "VIM";
    initscr();
    set_title(title);
    keypad(stdscr, TRUE);
    noecho();
    nodelay(stdscr, TRUE);

    ctrl = cut_ctrl;
    ctrl->subscribe(this);
    ctrl->set_pos(&t_y, &row_occupied);
    ctrl->set_point(&text_indexes);

    shift_x = 0;
    row_start = 0;
    win_hei = 0;
    shift_y = 0;
    row_occupied = 0;
    win_wid = 0;
    t_x = 0;
    t_y = 0;
    alive = true;
    mode = Common;

    sub_view = new CommonView(&shift_y, &shift_x,
        &row_start, &win_hei, &row_occupied,
        &win_wid, &w_y, &t_y, &text_indexes);
    m_state_line = new ViewStateLine(t_x, t_y, win_wid, win_hei, 4);
    keyboard = new Keyboard();

    w_y = shift_y;
    w_x = shift_x;
  
    keyboard->set_submodele(mode);
    keyboard->view_subscribe(this);
    keyboard->ctrl_subscribe(ctrl);
}

void View::main_loop() {
    while (alive) {
        update_window_information();
        keyboard->keyboard_handle();
    }
}

void View::update_text() {
    sub_view->markup(ctrl->get_text());
    m_state_line->display();
    sub_view->set_cursor(w_x, w_y);
}

void View::change_view() {
    delete sub_view;
    if (mode == Common) {
        sub_view = new HexView(&shift_y, &shift_x,
            &row_start, &win_hei, &row_occupied,
            &win_wid, &text_indexes);
        mode = Hex;
        keyboard->set_submodele(mode);
        row_start = 0;
        t_x = 0;
        t_y = 0;
        w_x = shift_x;
        w_y = shift_y;
    }
    else {
        sub_view = new CommonView(&shift_y, &shift_x,
            &row_start, &win_hei, &row_occupied,
            &win_wid, &w_y, &t_y, &text_indexes);
        mode = Common;
        keyboard->set_submodele(mode);
        row_start = 0;
        t_x = 0;
        t_y = 0;
        w_x = shift_x;
        w_y = shift_y;
    }
    update_window_information();
    update_text();
}

void View::display() {
    sub_view->display(ctrl->get_text());
    m_state_line->display();
    sub_view->set_cursor(w_x, w_y);
}

void View::shift_up_cursor() {
    if (w_y > shift_y) {
        if (w_y < shift_y+3 && row_start) {
            row_start--;
            t_y--;
        }
        else {
            w_y--;
            t_y--;
        }
        if (text_indexes.size() < w_y) {
            t_x = 0;
            w_x = shift_x;
        }
        else if (text_indexes[t_y][2] < w_x)
        {
            w_x = text_indexes[t_y][2]+shift_x-1;
            t_x = text_indexes[t_y][2]-1;
        }
    }
    m_state_line->update_position(t_x, t_y);
    display();
}

void View::shift_down_cursor() {
    if (row_occupied && w_y < win_hei && t_y < row_occupied - 1) {
        if (w_y + 4 > win_hei && row_start + win_hei - shift_y < row_occupied - 1)
        {
            row_start++;
            t_y++;
        }
        else {
            t_y++;
            w_y++;
        }
        if (text_indexes.size() <= t_y) {
            t_x = 0;
            w_x = shift_x;
        }
        else if (text_indexes[t_y][2] < w_x) {
            t_x = text_indexes[t_y][2]-1;
            w_x = text_indexes[t_y][2]+shift_x-1;
        }
    }
    m_state_line->update_position(t_x, t_y);
    display();
}

void View::shift_right_cursor() {
    if (text_indexes.size() > t_y)
        if (text_indexes[t_y][2] - 1 > t_x) {
            w_x++;
            t_x++;
        }
    m_state_line->update_position(t_x, t_y);
    sub_view->set_cursor(w_x, w_y);
}

void View::shift_left_cursor() {
    if (w_x > shift_x) {
        w_x--;
        t_x--;
    }
    m_state_line->update_position(t_x, t_y);
    sub_view->set_cursor(w_x, w_y);
}

void View::shift_begin_file() {
    row_start = 0;
    w_y = shift_y;
    w_x = shift_x;
    t_y = 0;
    t_x = 0;
    sub_view->markup(ctrl->get_text());
    m_state_line->update_position(t_x, t_y);
    display();
}

void View::shift_end_file() {
    t_y = row_occupied - 1;
    t_x = text_indexes[t_y][2]- 1;
    w_x = text_indexes[t_y][2]+shift_x -1;
    if (t_y >= win_hei) {
        w_y = win_hei;
        row_start = t_y - w_y + shift_y;
    }
    else w_y = t_y;
    m_state_line->update_position(t_x, t_y);
    display();
}

void View::shift_end_word() {
    if (text_indexes.size() > t_y)
        if (text_indexes[t_y][2] > t_x) {
            unsigned int i = t_y;
            unsigned int j = t_x;
            unsigned int f_idx = text_indexes[i][0];
            unsigned int len = text_indexes[i][2];
            string_t* data = ctrl->get_text();
            string_t substring = data->substr(f_idx, len);
            if (substring[j]) j++;
            short new_x = substring.find(" ", j);
            if (new_x < 0 || new_x >len)
                new_x = text_indexes[t_y][2] - 1;
            w_x = new_x + shift_x;
            t_x = new_x;
        }
    m_state_line->update_position(t_x, t_y);
    display();
}

void View::shift_begin_word() {
    if (text_indexes.size() >= t_y)
        if (t_x > 0) {
            unsigned int i = t_y;
            unsigned int j = t_x;
            unsigned int f_idx = text_indexes[i][0];
            unsigned int len = text_indexes[i][2];
            string_t* text_indexes = ctrl->get_text();
            string_t substring = text_indexes->substr(f_idx, len);
            while (substring[j] == ' ') j--;
            while (j > 0 && substring[j] != ' ')
                j--;
            t_x = j;
            w_x = j+shift_x;
        }
    m_state_line->update_position(t_x, t_y);
    display();
}

void View::shift_page_up() {
    if (row_occupied < win_hei)
        return;
    if (t_y < win_hei)
        row_start = 0;
    else
        row_start = t_y - win_hei;
    w_y = shift_y;
    t_y = row_start;
    m_state_line->update_position(t_x, t_y);
    display();
}

void View::shift_page_down() {
    if (row_occupied <= win_hei)
        return;
    unsigned int last_start_row = row_occupied - win_hei;
    unsigned int predict_start_row_down = t_y + win_hei;
    if (predict_start_row_down > last_start_row)
        row_start = last_start_row;
    else
        row_start = predict_start_row_down;
    w_y = shift_y;
    t_y = row_start;
    m_state_line->update_position(t_x, t_y);
    display();
}

void View::shift_end_line() {
    if (text_indexes.size()) {
        w_x = text_indexes[t_y][2]+ shift_x - 1;
        t_x = text_indexes[t_y][2];
    }
    m_state_line->update_position(t_x, t_y);
    display();
}

void View::shift_begin_line() {
    w_x = shift_x;
    t_x = 0;
    m_state_line->update_position(t_x, t_y);
    display();
}

void View::set_cursor(unsigned int idx) {
    unsigned int new_y = 0;
    if (text_indexes[text_indexes.size() - 1][1] <= idx) return;
    for (new_y; idx > text_indexes[new_y][1]+1; new_y++);

    if (idx - text_indexes[new_y][0] > text_indexes[new_y][2])
        new_y++;

    t_x = idx - text_indexes[new_y][0] - 1;
    t_y = new_y;
    w_x = idx - text_indexes[new_y][0] + shift_x - 1;

    if (new_y < win_hei / 2) {
        row_start = 0;
        w_y = new_y + shift_y;
    }
    else {
        row_start = new_y - win_hei / 2;
        w_y = shift_y + win_hei / 2;
    }
    m_state_line->update_position(t_x, t_y);
    display();
}

unsigned int View::get_current_index() {
    unsigned int i = t_y;
    unsigned int j = t_x;
    unsigned int idx = 0;
    if (text_indexes.size() != 0)
        idx = text_indexes[i][0] + j;
    return idx;
}

void View::update_window_information() {
    int row, col;
    //take up space for the info string
    if (is_resize(win_wid, win_hei + 1)) {
        unsigned short bias = 1;
        get_maxxy(&col, &row);
        m_state_line->update_dimensions(col, row);
        win_wid = col;
        win_hei = row - bias;
        if (t_x >= win_wid) {
            t_x = win_wid - 4;
            w_x = t_x;
        }
        m_state_line->update_position(t_x, t_y);
        display();
    }
}

void View::update_state_line(const char* cur_state) {
    string_t new_status = cur_state;
    m_state_line->update_status(new_status);
    sub_view->set_cursor(w_x, w_y);
}

void View::update_state_line(const string_t cur_state) {
    m_state_line->update_status(cur_state);
    sub_view->set_cursor(w_x, w_y);
}

void View::update_mode(const string_t cur_mode) {
    m_state_line->update_mode(cur_mode);
    sub_view->set_cursor(w_x, w_y);
}

void View::append_state_line(const char ch) {
    string_t new_status = m_state_line->get_state_line();
    new_status.append(1, ch);
    m_state_line->update_status(new_status);
    sub_view->set_cursor(w_x, w_y);
}

void View::kill() {
    alive = false;
}

void View::search(direction dir, const string_t& buffer) {
    string_t substring = buffer;
    substring.erase(0, 1);

    int f_idx = 0;
    unsigned int idx = get_current_index();
    if (dir == RIGHT) {
        shift_right_cursor();
        idx = get_current_index()+1;
        f_idx = ctrl->search(substring, idx, false);
    }
    else {
        f_idx = ctrl->search(substring, idx, true);
    }
    if (f_idx != -1) set_cursor(f_idx+1);
    else  shift_begin_file();
}

void View::delete_last_char() {
    string_t cur_status = m_state_line->get_state_line();
    move(w_y, w_x);
    cur_status.erase(cur_status.length() - 1, 1);
    m_state_line->update_status(cur_status);
    update_text();
}


void View::shift_specific_column(unsigned int idx) {
    if (idx < t_x)
        for (int i = t_x; i >= idx; i--)
            shift_left_cursor();
    else if (idx > t_x)
        for (int i = t_x; i < idx; i++)
            shift_left_cursor();
    display();
}

void View::shift_specific_line(unsigned int idx) {
    if (idx >= row_occupied) return;
    unsigned int x = t_x;
    for (unsigned int i = t_y; i < idx && i < row_occupied; i++)
        shift_down_cursor();
    shift_specific_column(x);
    display();
}