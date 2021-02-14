#ifndef _MY_VIM_H_
#define _MY_VIM_H_

#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include "curses.h"
#include <windows.h>
#include <strsafe.h>
#include <iostream>
#include <vector>
#include <stdlib.h>

#include "MyString.h"
enum direction { RIGHT, LEFT };
enum view { Common, Hex };

using string_t = MyString::String;
using triplet_t = std::vector<unsigned int>;
using feature_t = std::vector<triplet_t>;

std::ostream& operator<< (std::ostream& out, string_t& obj);
std::istream& operator>> (std::istream& in, string_t& obj);

class AdapterPDCurses {
public:
    static const short KEY_ESC = 27;
    static const short KEY_ENTR = 10;
    static const short KEY_BCKSPC = 8;
    static const short KEY_F5 = 269;
    AdapterPDCurses();
    void set_title(const string_t& title);
    int set_string(unsigned short y, unsigned short x, const string_t& text);
    int set_string(unsigned short y, unsigned short x, char text);
    void get_maxxy(int* x, int* y);
    void partial_cleaning(int first_y, int num_row);
    bool is_resize(int prev_x, int prev_y);
};

interface ModelNotification {
public:
    virtual void update_state_line(const char* cur_state) = 0;
    virtual void update_state_line(const string_t cur_state) = 0;
    virtual void update_text() = 0;
};

interface ViewKeyboardNotification {
public:
    virtual void shift_up_cursor() = 0;
    virtual void shift_down_cursor() = 0;
    virtual void shift_left_cursor() = 0;
    virtual void shift_right_cursor() = 0;
    virtual void shift_begin_file() = 0;
    virtual void shift_end_file() = 0;
    virtual void shift_end_line() = 0;
    virtual void shift_begin_line() = 0;
    virtual void shift_begin_word() = 0;
    virtual void shift_page_up() = 0;
    virtual void shift_page_down() = 0;
    virtual void shift_end_word() = 0;
    virtual void shift_specific_column(unsigned int idx) = 0;
    virtual void set_cursor(unsigned int idx) = 0;
    virtual void shift_specific_line(unsigned int idx) = 0;
    virtual unsigned int get_current_index() = 0;
    virtual void update_state_line(const char* cur_state) = 0;
    virtual void update_state_line(const string_t cur_state) = 0;
    virtual void update_mode(const string_t cur_mode) = 0;
    virtual void append_state_line(const char ch) = 0;
    virtual void search(direction dir, const string_t& buffer) = 0;
    virtual void display() = 0;
    virtual void delete_last_char() = 0;
    virtual void change_view() = 0;
    virtual void kill() = 0;
};

interface ControllerKeyboardNotification {
    virtual string_t read_text(const string_t& file_name) = 0;
    virtual string_t save_text(const string_t& file_name) = 0;
    virtual string_t save_text(const char* file_name) = 0;
    virtual void cut_string() = 0;
    virtual void delete_word(unsigned int) = 0;
    virtual void copy_word(unsigned int) = 0;
    virtual void delete_letter(unsigned int) = 0;
    virtual unsigned int paste(unsigned int) = 0;
    virtual void copy_string() = 0;
    virtual string_t set_help_file() = 0;
    virtual void set_buffer(const char ch) = 0;
    virtual void set_letter(const char ch) = 0;
    virtual void return_default_text() = 0;
};

interface IKeyboardSubmodule {
public:
    virtual string_t search(char*) = 0;
    virtual void bias(ViewKeyboardNotification* observer, unsigned int num) = 0;
};

class HexKeyboardSubmodule : public IKeyboardSubmodule {
    string_t search(char*) override;
    void bias(ViewKeyboardNotification* observer,unsigned int num);
};

class CommonKeyboardSubmodule : public IKeyboardSubmodule {
    string_t search(char*) override;
    void bias(ViewKeyboardNotification* observer,unsigned int num);
};

class Keyboard : AdapterPDCurses {
public:
    Keyboard();
    ~Keyboard();
    void keyboard_handle();
    void set_submodele(int mode);
    void view_subscribe(ViewKeyboardNotification* observer);
    void ctrl_subscribe(ControllerKeyboardNotification* observer);
private:
    enum mode { EditorMode, SearchMode, CommandMode, InputMode };
    const std::vector <string_t> mas_name_mode{ "<NAVIGATION AND EDITING>", "<SEARCH>", "<COMMAND>", "<INPUT>" };
    ViewKeyboardNotification* view_observer;
    ControllerKeyboardNotification* ctrl_observer;
    IKeyboardSubmodule* submodule;
    string_t input_buffer;
    bool is_change;
    string_t name_mode;
    //for search
    bool is_input;
    direction m_search;
    enum mode_command { STANDART, HELPER } m_command;
    //for command 'r'
    int limit_input;
    void handle_editor(int ch);
    void handle_search(int ch);
    void handle_command(int ch);
    void handle_helper(int ch);
    void handle_input(int ch);
    void split_command();
    void set_letter(const char ch);
    void bias();
};

class ViewStateLine : AdapterPDCurses {
public:
    ViewStateLine(unsigned int x, unsigned int y, unsigned int width, unsigned int high, unsigned int shift);
    void update_position(unsigned int x, unsigned int y);
    void update_mode(const string_t& mode);
    void update_status(const string_t& new_status);
    void update_dimensions(unsigned int x, unsigned y);
    string_t& get_state_line();
    void display();
private:
    string_t state_line;
    string_t name_mode;

    unsigned int shift;

    unsigned int mode_len;
    unsigned int status_len;
    unsigned int x, y;
    unsigned int max_x, max_y;
};

class Model {
public:
    void subscribe(ModelNotification* observer);
    void delete_word(unsigned int idx);
    unsigned int copy_word(unsigned int idx);
    void delete_element(unsigned int idx);
    void cut_string(unsigned int f_idx, unsigned int len);
    void copy_string(unsigned int f_idx, unsigned int len);
    unsigned int paste(unsigned int);
    string_t* get_text();
    void set_text(const string_t new_text);
    void set_buffer(char in_information);
    int search_substring(const string_t& substr, unsigned int pos, bool reverse);
    int find_reverse(string_t str, const string_t& substring, int pos);
    int find_reverse(string_t str, const char ch, int pos);
protected:
    ModelNotification* m_observer;
    string_t data;
    string_t buffer;
};

class Controller: public ControllerKeyboardNotification, AdapterPDCurses {
public:
    Controller(Model* model);
    ~Controller();
  
    string_t read_text(const string_t& file_name);
    string_t* get_text();
    string_t save_text(const string_t& file_name);
    string_t save_text(const char* file_name);
    void subscribe(ModelNotification*);
    void cut_string();
    void delete_word(unsigned int);
    void copy_word(unsigned int);
    void set_buffer(const char ch);
    void delete_letter(unsigned int);
    unsigned int paste(unsigned int);
    void copy_string();
    string_t set_help_file();
    void set_point(feature_t*);
    void set_pos(unsigned int*, unsigned int* );
    void set_letter(const char ch);
    void return_default_text();

    unsigned int search(const string_t& sub_string, unsigned int idx, bool flag);
private:
    string_t m_file;
    feature_t* p_layout;
    unsigned int* max_pos;
    unsigned int* p_pos;
    //for helper mode
    string_t temporary_buffer;
    Model* m_model;
};

interface IView  {
    virtual void display(string_t* data) = 0;
    virtual void markup(string_t* data) = 0;
    virtual void set_cursor(unsigned int, unsigned int) = 0;
};

class View: AdapterPDCurses, public ModelNotification, ViewKeyboardNotification {
public:
    View(Controller* cut_ctrl);

    void update_state_line(const char* cur_state) override;
    void update_state_line(const string_t cur_state) override;
    void update_text() override;

    void main_loop();
    void change_view();
    void update_window_information();

    void shift_up_cursor() override;
    void shift_down_cursor() override;
    void shift_left_cursor() override;
    void shift_right_cursor() override;
    void shift_begin_file() override;
    void shift_end_file() override;
    void shift_end_line() override;
    void shift_begin_line() override;
    void shift_begin_word() override;
    void shift_page_up() override;
    void shift_page_down() override;
    void shift_end_word() override;
    void shift_specific_column(unsigned int idx) override;
    void set_cursor(unsigned int idx) override;
    void shift_specific_line(unsigned int idx) override;
    unsigned int get_current_index() override;
    void update_mode(const string_t cur_mode) override;
    void append_state_line(const char ch) override;
    void search(direction dir, const string_t& buffer) override;
    void display() override;
    void delete_last_char() override;
    void kill() override;
private:
    unsigned int shift_y, shift_x;
    unsigned int row_start;
    unsigned int w_x, w_y;
    unsigned int t_x, t_y;

    unsigned int win_hei, win_wid;
    unsigned int row_occupied;
    bool alive;

    feature_t text_indexes;
    ViewStateLine* m_state_line;
    Keyboard* keyboard;
    IView* sub_view;
    Controller* ctrl;
    int mode;
};

class CommonView: public IView, AdapterPDCurses {
public:
    CommonView(unsigned int* row_shift, unsigned int* column_shift,
        unsigned int* start, unsigned int* cur_win_rows, unsigned int* cur_text_rows,
        unsigned int* cur_win_column, unsigned int* cur_win_y, unsigned int* cur_text_y,
        feature_t* cur_markup);

    void display(string_t* data);
    void markup(string_t* data);
    void set_cursor(unsigned int, unsigned int);
private:
private:
    unsigned int* shift_y, *shift_x;
    unsigned int* row_start, *win_rows, *text_rows;
    unsigned int* win_column;
    unsigned int* win_y, *text_y;
    feature_t* text;
    int find_reverse(string_t str, const char ch, int pos);
};

class HexView: public IView, AdapterPDCurses {
public:
    HexView(unsigned int* row_shift, unsigned int* column_shift,
        unsigned int* start, unsigned int* cur_win_rows, unsigned int* cur_text_rows,
        unsigned int* cur_win_column, feature_t* cur_markup);

    void display(string_t* data);
    void markup(string_t* data);
    void set_cursor(unsigned int, unsigned int);
private:
    unsigned int width_data;
    unsigned int width_hex;
    unsigned int step;

    unsigned int* shift_y, *shift_x;
    unsigned int* row_start, *win_rows, *text_rows;
    unsigned int* win_column;

    feature_t* text;
};

#endif