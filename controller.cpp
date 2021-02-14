#include "core.h"

Controller::~Controller() {
    clear();
    refresh();
}

Controller::Controller(Model* model) {
    m_model = model;
}

void Controller::subscribe(ModelNotification* observer) {
    m_model->subscribe(observer);
}

string_t Controller::read_text(const string_t& file_name) {
    string_t ret;
    if (file_name == "None") {
        ret = "File cann't been opened";
        return ret;
    }
    m_file = file_name;
    std::ifstream text_file(file_name.c_str());

    if (!text_file.is_open()) {
        ret = "[] File cann't been opened";
        ret.insert(1, file_name.c_str());
        return ret;
    }

    string_t new_data;
    text_file >> new_data;

    ret = "[] File is opened";
    ret.insert(1, file_name.c_str());

    text_file.close();
    m_model->set_text(new_data);

    return ret;
}

string_t* Controller::get_text() {
    return m_model->get_text();
}

string_t Controller::save_text(const string_t& file_name) {
    string_t* data = m_model->get_text();
    string_t ret;
    string_t f_name = file_name; //It is possible to change
    if (data->size() == 0) {
        ret = "Nothing to save";
        return ret;
    }
    if (file_name == "None")
        f_name = m_file;

    std::ofstream file_save(f_name.c_str());
    if (file_save.is_open()) {
        file_save << *data;
        ret = "Changes are saved in ";
        ret.append(f_name.c_str());
    }
    else ret = "Error save!";
    return ret;
}

string_t Controller::save_text(const char* file_name) {
    string_t name(file_name);
    return save_text(name);
}

void Controller::set_point(feature_t* point) {
    p_layout = point;
}

void Controller::set_pos(unsigned int* pos, unsigned int* max) {
    p_pos = pos;
    max_pos = max;
}

void Controller::cut_string() {
    if (*max_pos == 0) return;
    unsigned int f_idx = (*p_layout)[*p_pos][0],
        len= (*p_layout)[*p_pos][2];
    m_model->cut_string(f_idx, len);
}

void Controller::delete_word(unsigned int idx) {
    m_model->delete_word(idx);
}

void Controller::copy_word(unsigned int idx) {
    m_model->copy_word(idx);
}

void Controller::delete_letter(unsigned int idx) {
    m_model->delete_element(idx);
}

unsigned int Controller::paste(unsigned int idx) {
    unsigned int res = m_model->paste(idx);
    return res + idx;
}

void Controller::copy_string() {
    if (*max_pos == 0) return;
    unsigned int f_idx = (*p_layout)[*p_pos][0],
        len = (*p_layout)[*p_pos][2];
    m_model->copy_string(f_idx, len);
}

string_t Controller::set_help_file() {
    temporary_buffer = *m_model->get_text();
    string_t helper_file = "help.txt";
    read_text(helper_file);
    return "HELP SECTION";
}

//jump to another line
void Controller::set_letter(const char ch) {
   
}

void Controller::set_buffer(const char ch) {
    m_model->set_buffer(ch);
}

unsigned int Controller::search(const string_t& sub_string, unsigned int idx, bool flag) {
    return m_model->search_substring(sub_string, idx, flag);
}

void Controller::return_default_text() {
    m_model->set_text(temporary_buffer);
    temporary_buffer.clear();
}