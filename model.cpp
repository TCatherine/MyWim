#include "core.h"

void Model::subscribe(ModelNotification* observer) {
    m_observer = observer;
}

void Model::delete_element(unsigned int idx) {
    if (data.size() == 0) return;
    char del_ch[] = { data[idx], 0 };
    string_t information = "[] is deleted";
    information.insert(1, del_ch);
    m_observer->update_state_line(information);
    data.erase(idx, 1);
    m_observer->update_text();
}

void Model::copy_string(unsigned int f_idx, unsigned int len) {
    buffer.clear();
    buffer = data.substr(f_idx, len);
    if (data[f_idx + len] == '\n')
        len++;
    m_observer->update_state_line("String is copied");
    m_observer->update_text();
}

void Model::cut_string(unsigned int f_idx, unsigned int len) {
    copy_string(f_idx, len);
    data.erase(f_idx, len);
    m_observer->update_state_line("String is cut");
    m_observer->update_text();
}

void Model::delete_word(unsigned int idx) {
    int frs_ch = copy_word(idx);
    data.erase(frs_ch, buffer.length() + 1);
    string_t information = "[] is deleted";
    information.insert(1, buffer.c_str());
    m_observer->update_state_line(information);
    buffer.clear();
    m_observer->update_text();
    return;
}

unsigned int Model::copy_word(unsigned int idx) {
    buffer.clear();
    int frs_ch = idx;
    frs_ch = find_reverse(data, ' ', idx);
    int prob_frs_ch = find_reverse(data, '\n', idx);
    if (prob_frs_ch > frs_ch) 
        frs_ch = prob_frs_ch;
    frs_ch++;
    unsigned int end_ch = data.find(" ", idx + 1);
    buffer = data.substr(frs_ch, end_ch - frs_ch);
    string_t information = "[] File is opened";
    information.insert(1, buffer.c_str());
    m_observer->update_state_line(information);
    return frs_ch;
}

unsigned int Model::paste(unsigned int idx) {
    if (buffer.size() == 0)
        return 0;
    data.insert(idx, buffer.c_str());
    string_t information = "[] is pasted";
    information.insert(1, buffer.c_str());
    unsigned int num_copy = buffer.length();
    buffer.clear();
    m_observer->update_state_line(information);
    m_observer->update_text();
    return num_copy;
}

void Model::set_buffer(char in_information) {
    buffer = in_information;
}

string_t* Model::get_text() {
    return &data;
}

int Model::search_substring(const string_t& substr, unsigned int pos, bool reverse) {
    if (reverse) return find_reverse(data, substr, pos);
    else return data.find(substr.c_str(), pos);
}

void Model::set_text(const string_t new_data) {
    data = new_data;
    m_observer->update_text();
}

int Model::find_reverse(string_t str, const string_t& substring, int pos) {
    short prev_idx = -1;
    short idx = str.find(substring.c_str());
    while (idx + substring.length() - 1 < pos && idx != -1) {
        prev_idx = idx;
        idx = str.find(substring.c_str(), prev_idx + 1);
    }
    return prev_idx;
}

int Model::find_reverse(string_t str, const char ch, int pos) {
    char s_substring[] = { ch, 0 };
    return find_reverse(str, s_substring, pos);
}