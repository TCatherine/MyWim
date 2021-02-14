#include "core.h"

Keyboard::Keyboard() {
    is_change = false;
    limit_input = 0;
    submodule = NULL;
    name_mode = mas_name_mode[EditorMode];
}

 Keyboard::~Keyboard() {
    delete ctrl_observer;
    delete view_observer;
}

 void Keyboard::set_submodele(int mode) {
     if (submodule != NULL) 
         delete submodule;
     if (mode == Common)
         submodule = new CommonKeyboardSubmodule;
     else 
         submodule = new HexKeyboardSubmodule;
 }

void Keyboard::view_subscribe(ViewKeyboardNotification* observer) {
    view_observer = observer;
    view_observer->update_mode(name_mode);
}

void Keyboard::ctrl_subscribe(ControllerKeyboardNotification* observer) {
    ctrl_observer = observer;
}

void Keyboard::bias() {
    short idx = input_buffer.find(" ");
    string_t argument;
    if (idx == -1)
        argument = input_buffer;
    else
        argument = input_buffer.substr(0, idx);
    int num = atoi(argument.c_str());

    submodule->bias(view_observer, num);
}

void Keyboard::handle_editor(int ch) {
    int num;
    string_t param = { (char)ch};
    switch (ch) {
    case 'w':
        if (input_buffer.find("di") != -1) {
            //diw - delete word
            input_buffer.clear();
            is_change = true;
            ctrl_observer->delete_word(view_observer->get_current_index());
        }
        else
        if (input_buffer.size() > 0 && input_buffer[input_buffer.length() - 1] == 'y') {
            //yw - copy word
            input_buffer.clear();
            ctrl_observer->copy_word(view_observer->get_current_index());
        }
        else view_observer->shift_end_word();
        break;
    case 'b':
        view_observer->shift_begin_word();
        break;
    case 'x':
        ctrl_observer->delete_letter(view_observer->get_current_index());
        view_observer->shift_left_cursor();
        is_change = true;
        break;
    case 'G':
        if (input_buffer.size() > 0) {
            num = atoi(input_buffer.c_str());
            input_buffer.clear();
            view_observer->shift_specific_line(num - 1);
        }
        else view_observer->shift_end_file();
        break;
    case '$':
        view_observer->shift_end_line();
        break;
    case '0':
        input_buffer.append(param.c_str());
        view_observer->append_state_line(ch);
    case '^':
        view_observer->shift_begin_line();
        break;
    case 'g':
        if (input_buffer.size() > 0 && input_buffer[input_buffer.length() - 1] == 'g') {
            input_buffer.clear();
            view_observer->shift_begin_file();
        }
        else {
            input_buffer.append("g");
        }
        break;
    case 'd':
        if (input_buffer.size() > 0 && input_buffer[input_buffer.length() - 1] == 'd') {
            //dd - cut string
            input_buffer.clear();
            ctrl_observer->cut_string();
            view_observer->shift_begin_line();
            is_change = true;
        }
        else {
            input_buffer.append("d");
        }
        break;
    case 'i':
        if (input_buffer.size() > 0 && input_buffer[input_buffer.length() - 1] == 'd')
            input_buffer.append("i");
        else {
            name_mode = mas_name_mode[InputMode];
            view_observer->update_mode(name_mode);
            ctrl_observer->set_buffer(0);
        }
        view_observer->display();
        break;
    case 'I':
        name_mode = mas_name_mode[InputMode];
        view_observer->update_state_line("");
        view_observer->update_mode(name_mode);
        view_observer->shift_begin_line();
        ctrl_observer->set_buffer(0);
        break;
    case 'A':
        name_mode = mas_name_mode[InputMode];
        view_observer->update_state_line("");
        view_observer->shift_end_line();
        view_observer->update_mode(name_mode);
        ctrl_observer->set_buffer(0);
        break;
    case 'S':
        name_mode = mas_name_mode[InputMode];
        input_buffer.clear();
        ctrl_observer->cut_string();
        view_observer->update_mode(name_mode);
        view_observer->shift_begin_line();
        is_change = true;
        break;
    case 'r':
        limit_input = 1;
        name_mode = mas_name_mode[InputMode];
        view_observer->update_mode(name_mode);
        ctrl_observer->delete_letter(view_observer->get_current_index());
        is_change = true;
        ctrl_observer->set_buffer(0);
        break;
    case 'y':
        ctrl_observer->copy_string();
        input_buffer.append("y");
        break;
    case 'p':
        view_observer->set_cursor
        (ctrl_observer->paste(view_observer->get_current_index()));
        is_change = true;
        break;
    case '/':
        name_mode = mas_name_mode[SearchMode];
        input_buffer.append("/");
        view_observer->update_state_line("/");
        view_observer->update_mode(name_mode);
        m_search = RIGHT;
        is_input = true;
        break;
    case '?':
        name_mode = mas_name_mode[SearchMode];
        input_buffer.append("?");
        m_search = LEFT;
        view_observer->update_mode(name_mode);
        view_observer->update_state_line("?");
        is_input = true;
        view_observer->display();
        break;
    case ':':
        name_mode = mas_name_mode[CommandMode];
        input_buffer.clear();
        is_input = false;
        m_command = STANDART;
        view_observer->update_mode(name_mode);
        view_observer->update_state_line("");
        break;
    case 'N':
        view_observer->update_state_line("");
        input_buffer.clear();
        is_input = true;
        break;
    case KEY_ENTR:
        if (is_input == true) {
            bias();
            view_observer->update_state_line("");
            input_buffer.clear();
            is_input = false;
        }
        break;
    case KEY_PPAGE:
        view_observer->shift_page_up();
        break;
    case KEY_NPAGE:
        view_observer->shift_page_down();
        break;
    default:
        if (ch >= '0' && ch <= '9' && is_input) {
            input_buffer.append(param.c_str());
            view_observer->append_state_line(ch);
        }
    }
}

void Keyboard::handle_search(int ch) {
    string_t param = { (char)ch};
    switch (ch) {
    case KEY_ENTR:
        is_input = false;
        input_buffer = submodule->search((char*)input_buffer.c_str());
        view_observer->search(m_search, input_buffer);
        break;
    case KEY_BCKSPC:
        if (input_buffer.length())
            input_buffer.erase(input_buffer.length() - 1, 1);
        view_observer->delete_last_char();
        is_input = true;
        break;
    case KEY_ESC: 
        name_mode = mas_name_mode[EditorMode];
        input_buffer.clear();
        view_observer->update_state_line("");
        view_observer->update_mode(name_mode);
        break;
    case 'n':
        if (!is_input) {
            view_observer->search(m_search, input_buffer);
        }
        else {
            view_observer->append_state_line(ch);
            input_buffer.append(param.c_str());
        }
        break;
    case 'N':
        if (!is_input) {
            if (m_search == RIGHT) {
                m_search = LEFT;
            }
            else {
                m_search = RIGHT;
            }
            view_observer->search(m_search, input_buffer);
            if (m_search == RIGHT)
                m_search = LEFT;
            else m_search = RIGHT;
        }
        else {
            view_observer->append_state_line(ch);
            input_buffer.append(param.c_str());
        }
        break;
    default: 
        if (is_input) {
            view_observer->append_state_line(ch);
            input_buffer.append(param.c_str());
        }
    }
}

void Keyboard::handle_helper(int ch) {
    switch (ch) {
    case KEY_UP:
        view_observer->shift_up_cursor();
        break;
    case KEY_DOWN:
        view_observer->shift_down_cursor();
        break;
    case KEY_RIGHT:
        view_observer->shift_right_cursor();
        break;
    case KEY_LEFT:
        view_observer->shift_left_cursor();
        break;
    case 'w':
        view_observer->shift_end_word();
        break;
    case 'b':
        view_observer->shift_begin_word();
        break;
    case 'G':
        view_observer->shift_end_file();
        break;
    case '$':
        view_observer->shift_end_line();
        break;
    case '0':
    case '^':
        view_observer->shift_begin_line();
        break;
    case 'g':
        if (input_buffer.size() > 0 && input_buffer[input_buffer.length() - 1] == 'g') {
            input_buffer.clear();
            view_observer->shift_begin_file();
        }
        else {
            input_buffer.append("g");
        }
        break;
    case KEY_ESC: 
        m_command = STANDART;
        name_mode = mas_name_mode[EditorMode];
        view_observer->update_mode(name_mode);
        ctrl_observer->return_default_text();
        view_observer->update_state_line("");
        view_observer->shift_begin_file();
        break;
    case KEY_PPAGE:
        view_observer->shift_page_up();
        break;
    case KEY_NPAGE:
        view_observer->shift_page_down();
        break;
    }
}

void Keyboard::handle_command(int ch) {
    string_t param = { (char)ch };
    if (m_command == STANDART) {
        switch (ch) {
        case KEY_BCKSPC:
            if (input_buffer.length())
                input_buffer.erase(input_buffer.length() - 1, 1);
            view_observer->delete_last_char();
            break;
        case KEY_ENTR:
            is_input = false;
            split_command();
            if (m_command == HELPER)
                break;
        case KEY_ESC: 
            name_mode = mas_name_mode[EditorMode];
            input_buffer.clear();
            view_observer->update_mode(name_mode);
            view_observer->display();
            break;
        default:
            if (is_input == false) {
                is_input = true;
                view_observer->update_state_line("");
            }
            view_observer->append_state_line(ch);
            input_buffer.append(param.c_str());
        }
        view_observer->display();
    }
    else handle_helper(ch);
}

void Keyboard::keyboard_handle()
{
    int ch = getch();
    if (ch == -1) 
        return;
    switch (ch) 
    {
    case KEY_F5:
        view_observer->change_view();
        break;
    case KEY_UP:
        view_observer->shift_up_cursor();
        break;
    case KEY_DOWN:
        view_observer->shift_down_cursor();
        break;
    case KEY_RIGHT:
        view_observer->shift_right_cursor();
        break;
    case KEY_LEFT:
        view_observer->shift_left_cursor();
        break;
    default:
        if (name_mode == mas_name_mode[EditorMode].c_str())
            handle_editor(ch);
        else if (name_mode == mas_name_mode[SearchMode].c_str())
            handle_search(ch);
        else if (name_mode == mas_name_mode[CommandMode].c_str())
            handle_command(ch);
        else if (name_mode == mas_name_mode[InputMode].c_str())
            handle_input(ch);
    }
}

void Keyboard::set_letter(const char ch) {
    ctrl_observer->set_buffer(ch);
    view_observer->set_cursor
    (ctrl_observer->paste(view_observer->get_current_index()));
    if (ch == '\n') {
        view_observer->shift_begin_line();
    }
}

void Keyboard::handle_input(int ch)
{
    switch (ch) {
    case KEY_BCKSPC:
        ctrl_observer->delete_letter(view_observer->get_current_index()-1);
        view_observer->shift_left_cursor();
        is_change = true;
        break;
    case KEY_ESC:
        name_mode = mas_name_mode[EditorMode];
        input_buffer.clear();
        view_observer->update_mode(name_mode);
        view_observer->update_state_line("");
        break;
    case KEY_ENTR:
        set_letter('\n');
        is_change = true;
        break;
    default:
        //for other case
        set_letter(ch);
        is_change = true;
        //m_observer->shift_right_cursor();
        if (limit_input == 1) {
            name_mode = mas_name_mode[EditorMode];
            view_observer->update_mode(name_mode);
            input_buffer.clear();
        }
    }
}

void  Keyboard::split_command() {
    string_t argument;
    string_t file;
    short idx = input_buffer.find(" ");
    if (idx == -1)
        argument = input_buffer;
    else {
        argument = input_buffer.substr(0, idx);
        file = input_buffer.substr(idx + 1, input_buffer.length() - idx - 1);
    }
    if (argument == "x" || argument == "wq!") {
        if (file.size() != 0) {
            view_observer->update_state_line("Error command!");
            return;
        }
        view_observer->update_state_line
        (ctrl_observer->save_text("None"));
        view_observer->kill();
    }
    else if (argument == "w") {
        if (file.size() != 0)
            view_observer->update_state_line
            (ctrl_observer->save_text(file));
        else view_observer->update_state_line
        (ctrl_observer->save_text("None"));
    }
    else if (argument == "q") {
        if (file.size() != 0) {
            view_observer->update_state_line("Error command!");
            return;
        }
        if (is_change) {
            view_observer->update_state_line("The file has been modified!");
            return;
        }
        else {
            view_observer->kill();
        }
    }
    else if (argument == "q!") {
        if (file.size() != 0) {
            view_observer->update_state_line("Error command!");
            return;
        }
        view_observer->kill();
    }
    else if (argument == "o") {
        if (file.size() == 0) {
            view_observer->update_state_line("Error command!");
            return;
        }
        view_observer->update_state_line(ctrl_observer->read_text(file));
    }
    else if (argument == "h") {
        view_observer->update_mode
        (ctrl_observer->set_help_file());
        m_command = HELPER;
    }
    else {
        bias();
    }
}

string_t CommonKeyboardSubmodule::search(char* substring) {
    return substring;
}

string_t HexKeyboardSubmodule::search(char* substr) {
    string_t substring(substr);
    string_t text;
    std::vector<int> code;

    if (substring[1]!='\\' || substring[2]!='x')
        return substring;

    for (unsigned int i = 3; i < substring.length(); i++) {
        if (substring[i] >= '0' && substring[i] <= '9')
            code.push_back(substring[i] - '0');
        else if (substring[i] <= 'f' && substring[i] >= 'a')
            code.push_back(substring[i] - 'a' + 10);
        else
            return substring;
    }

    if (!(substring.length() % 2))
        substring.erase(substring.length() - 1, 1);

    for (unsigned int i = 0; i < code.size(); i += 2) {
        char letter = code[i] * 16 + code[i + 1];
        text.push_back(letter);
    }
    text.insert(0, "x");
    return text;
}

void CommonKeyboardSubmodule::bias(ViewKeyboardNotification* observer, unsigned int num) {
    observer->shift_specific_line(num - 1);
}

void HexKeyboardSubmodule::bias(ViewKeyboardNotification* observer, unsigned int num) {
    observer->set_cursor(num+1);
}