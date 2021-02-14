#include "core.h"

HexView::HexView(unsigned int* row_shift, unsigned int* column_shift,
    unsigned int* start, unsigned int* cur_win_rows, unsigned int* cur_text_rows,
    unsigned int* cur_win_column,feature_t* cur_markup) {
    width_data = 0;
    width_hex = 0;
    step = 0;

    shift_y = row_shift;
    shift_x = column_shift;

    *shift_y = 7;
    *shift_x = 1;

    row_start = start;
    win_rows = cur_win_rows;
    text_rows = cur_text_rows;
    win_column = cur_win_column;

    text = cur_markup;
}

CommonView::CommonView(unsigned int* row_shift, unsigned int* column_shift,
    unsigned int* start, unsigned int* cur_win_rows, unsigned int* cur_text_rows,
    unsigned int* cur_win_column, unsigned int* cur_win_y, unsigned int* cur_text_y,
   feature_t* cur_markup)
{
    text = cur_markup;

    win_y = cur_win_y; 
    text_y = cur_text_y;

    shift_y = row_shift;
    shift_x = column_shift;

    *shift_y = 5;
    *shift_x = 0;

    row_start = start;
    win_rows = cur_win_rows;
    text_rows = cur_text_rows;
    win_column = cur_win_column;

}

void HexView::display(string_t* data) {
    partial_cleaning(*shift_y-2, *win_rows - *shift_y + 3);
    width_data = 18;
    width_hex = *win_column - width_data;
    step = width_hex / 16;
    if (step * 16 < width_hex)
        width_hex = step * 17;
    width_data = *win_column - width_hex;

    char inf_line[] = { '#', '0', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    unsigned int y = *shift_y;
    for (unsigned int i = 0; i < 18; i++)
        set_string(y-2, i*step, inf_line[i]);
    set_string(y - 2, step/2, '|');
    set_string(y - 2, width_hex, '|');
    set_string(y - 2, *win_column * 0.9 - 4, "DATA");
    string_t border(*win_column, '-');
    set_string(y - 1, 0, border);

    for (unsigned int num_field = *row_start, y_pos = y; y_pos <= *win_rows && num_field < *text_rows; num_field++, y_pos+=1) {
        char str_number[10];
        _itoa(num_field, str_number, 10);
        string_t number(str_number);
        set_string(y_pos, 0, number);
        set_string(y_pos, step / 2, '|');
        set_string(y_pos, width_hex, '|');
    }

    unsigned short x = width_hex+1;
    unsigned int i;
    unsigned j;
    string_t substring;
    for (i = y, j = *row_start; i <= *win_rows && j < *text_rows; i++, j++) {
        y = i;
        unsigned int f_idx = (*text)[j][0];
        unsigned int len = (*text)[j][2];
        substring = data->substr(f_idx, len);
        set_string(y, x, substring);
        for (unsigned int u = 0; u < 16; u++) {
            if (u < substring.length()) {
                char buffer[4] = { 0 };
                _itoa(substring[u], buffer, 16);
                set_string(y, (u+*shift_x) * step, buffer);
            }
            else
                set_string(y, (u+*shift_x) * step, "--");
        }
    }
}

void HexView::markup(string_t* data){
    unsigned int pos = 0;
    text->clear();
    *text_rows = 0;
    unsigned int len_string = 16;
    while (data->size() != 0 && data->length() > pos) {
        unsigned int new_pos = pos;
        unsigned int num_string = 0;
        int end_string = data->find("\n", pos);
        if (end_string == -1 || end_string > pos + len_string) {
            end_string = pos + len_string;
            num_string = len_string;
        }
        else {
            num_string = end_string - pos+1;
        }
        triplet_t new_string = { pos, (unsigned int)end_string, num_string};
        text->push_back(new_string);
        (*text_rows)++;
        pos = new_pos + num_string;
    }
    display(data);
}

void HexView::set_cursor(unsigned int x, unsigned int y) {
    move(y, x * step);
    refresh();
}

void CommonView::display(string_t* data) {
    partial_cleaning(*shift_y, *win_rows-*shift_y + 1);

    DWORD l;
    LPVOID lr = NULL;
    unsigned short x = 0, y = *shift_y;

    if (data->size() != 0) {
        unsigned int i;
        unsigned j;
        x = 0;
        string_t substring;
        for (i = y, j = *row_start; i <= *win_rows && j < *text_rows; i++, j++) {
            y = i;
            unsigned int f_idx = (*text)[j][0];
            unsigned int len = (*text)[j][2];
            substring = data->substr(f_idx, len);
            set_string(y, x, substring);
        }
    }
}

void CommonView::markup(string_t* data) {
    unsigned int pos = 0;
    text->clear();
    (*text_rows) = 0;
    while (data->size() != 0 && data->length() > pos) {
        int end_string = data->find("\n", pos);
        if (end_string == -1)
            end_string = data->length();
        unsigned int len_string = end_string - pos;
        unsigned int num_letters = 0;
        unsigned int new_pos = pos;
        if (len_string < *win_column) {
            num_letters = len_string;
            new_pos += 1;
        }
        else {
            //if size of the windows don't allow fit the whole line
            end_string = find_reverse(*data, ' ', *win_column + pos);
            if (end_string == -1) {
                num_letters = *win_column - 1;
            }
            else num_letters = end_string - pos + 1;
        }
        triplet_t new_string = { pos, (unsigned int)end_string, num_letters+1};
        text->push_back(new_string);
        (*text_rows)++;
        pos = new_pos + num_letters;
        if (data->length() == pos && data->operator[](pos - 1) == '\n') {
            triplet_t new_string = { pos, pos, 1 };
            text->push_back(new_string);
            (*text_rows)++;
        }
    }
    if ((*text_y) >= (*text_rows) && (*text_y) > 0) {
        unsigned int del = *text_y - (*text_rows) +1;
        *text_y = *text_rows - 1;
        if (*win_y < del) {
            *row_start -= del;
        }
        else win_y -= del;
    }
    display(data);
}

void CommonView::set_cursor(unsigned int x, unsigned int y) {
    move(y, x);
    refresh();
}

int CommonView::find_reverse(string_t str, const char ch, int pos) {
    string_t substring(1, ch);
    short prev_idx = -1;
    short idx = str.find(substring.c_str());
    while (idx + substring.length() - 1 < pos && idx != -1) {
        prev_idx = idx;
        idx = str.find(substring.c_str(), prev_idx + 1);
    }
    return prev_idx;
}