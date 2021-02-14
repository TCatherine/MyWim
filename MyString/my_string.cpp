#include "pch.h"
#include "framework.h"
#include "my_string.h"

int MyString::value_capacity(int value) {
	unsigned int block_count = value / BLOCK_SIZE + 1;
	unsigned int capacity = BLOCK_SIZE * block_count;
	return capacity;
}

MyString::MyString() {
	m_capacity = BLOCK_SIZE;
	m_data = new char[m_capacity];
	m_data[0] = 0;
	m_len = 0;
}

bool MyString::check_index(int value) {
	return value > 0;
}

void MyString::copy_string(const char* input_data) {
	m_capacity = value_capacity(m_len);
	m_data = new char[m_capacity];
	unsigned int i = 0;
	for (i = 0; i < m_len; i++)
		m_data[i] = input_data[i];
	m_data[i] = 0;
}

MyString::MyString(const MyString& input_data) {
	m_len = input_data.lenght();
	copy_string(input_data.c_str());
}

MyString::MyString(const std::initializer_list<char>& input_data) {
	m_capacity = value_capacity(input_data.size()+1);
	m_data = new char[m_capacity];
	unsigned int count = 0;
	for (auto element : input_data)
	{
		m_data[count] = element;
		++count;
	}
	m_data[count] = 0;
	m_len = count;
}

MyString::MyString(const char* input_data) {
	//count m_len
	if (input_data == NULL)
	{
		m_len = 0;
		m_capacity = BLOCK_SIZE;
		m_data = new char[m_capacity];
		m_data[0] = 0;
		return;
	}
	unsigned int m_len_input = 0;
	for (m_len_input; input_data[m_len_input]!='\0'; m_len_input++); 
	m_len = m_len_input;

	m_capacity = value_capacity(m_len_input + 1);
	m_data = new char[m_capacity];
	unsigned int i = 0;
	for (i = 0; i < m_len && i < m_len_input; i++)
		m_data[i] = input_data[i];
	m_data[i] = 0;
}

MyString::MyString(const char* input_data, const int count) {
	if (!check_index(count)) {
		m_capacity = BLOCK_SIZE;
		m_len = 0;
		m_data = new char[m_capacity];
		m_data[0] = 0;
		return;
	}
	m_len = count;
	copy_string(input_data);
}

MyString::MyString(const std::string& input_data) {
	m_len = input_data.length();
	copy_string(input_data.c_str());
}

MyString::MyString(const int count, const char ch) {
	if (!check_index(count)) {
		m_len = 0;
		m_capacity = BLOCK_SIZE;
		m_data = new char[m_capacity];
		m_data[0] = 0;
		return;
	};
	m_len = count;
	m_capacity = value_capacity(count + 1);
	m_data = new char[m_capacity];
	unsigned int i;
	for (i = 0; i < count; i++)
		m_data[i] = ch;
	m_data[i] = 0;
}

MyString::~MyString() {
	delete[] m_data;
	m_len = 0;
	m_capacity = 0;
	m_data = 0;
}

int MyString::lenght() const {
	return m_len;
}

int MyString::size() const {
	return m_len*sizeof(char);
}

char* MyString::c_str() const {
	return m_data;
}

char* MyString::data() const {
	return m_data;
}

bool MyString::empty() const {
	return size() == 0;
}

int MyString::capacity() const {
	return m_capacity * sizeof(char);
}

void MyString::clear() {
	m_data[0] = 0;
	m_len = 0;
}

void MyString::shrink_to_fit() {
	if (m_capacity <= m_len + 1)
		return;
	m_capacity = m_len + 1;
	char* new_string = new char[m_capacity];
	for (int i = 0; i < m_len; i++)
		new_string[i] = m_data[i];
	new_string[m_len] = 0;
	delete[] m_data;
	m_data = new_string;
}

void MyString::insert(int index, int count, char letter) {
	if (!check_index(count)) return;
	if (index < 0) return;
	unsigned int sub_capacity = count + 1;
	char* subline = new char[sub_capacity];
	for (unsigned int i = 0; i < count; i++)
		subline[i] = letter;
	subline[count] = 0;
	insert(index, subline);
	return;
}

void MyString::insert(int index, const char* subline) {
	if (index < 0) return;
	//count m_len
	int new_m_len = 0;
	for (new_m_len; subline[new_m_len]; new_m_len++);
	new_m_len += m_len;

	m_capacity = value_capacity(new_m_len);
	char* new_string = new char[m_capacity];
	for (unsigned int i = 0, i1 = 0; i < new_m_len; i++, i1++) {
		if (i == index) 
			for (unsigned int i2 = 0; subline[i2]; i2++, i++)
				new_string[i] = subline[i2]; //write substring
		new_string[i] = m_data[i1]; //rewrite
	}
	new_string[new_m_len] = 0;
	delete[] m_data;
	m_data = new_string;
	m_len = new_m_len;
	return;
}

void MyString::insert(int index, const char* subline, int count) {
	if (index < 0) return;
	if (!check_index(count)) return;
	unsigned int new_m_len = count;
	unsigned int sub_capacity = count + 1;
	char* new_subline = new char[sub_capacity];
	for (unsigned int i = 0; i < count; i++) 
		new_subline[i] = subline[i];
	new_subline[count] = 0;
	insert(index, new_subline);
	return;
}

void MyString::insert(const int index, const std::string& new_string) {
	if (index < 0) return;
	return insert(index, new_string.c_str());
}

void MyString::insert(const int index, const std::string& new_string, int count) {
	return insert(index, new_string.c_str(), count);
}

void MyString::erase(const int index, const int count) {
	if (!check_index(count)) return;
	if (index < 0) return;
	unsigned int new_m_len = m_len - count;
	char* new_string = m_data;

	for (int i = 0, i_old = 0; 
		i < new_m_len && i_old < m_len; 
		i++, i_old++) {
		if (i == index) 
			for (unsigned int i_count = 0;  
				i_count < count && i_old < m_len;
				i_count++, i_old++);

		new_string[i] = m_data[i_old];
	}

	new_string[new_m_len] = 0;
	m_len = new_m_len;
	return;
}

void MyString::append(const int count, const char ch) {
	if (!check_index(count)) return;
	unsigned int sub_capacity = count + 1;
	char* substring = new char[sub_capacity];
	for (unsigned int i = 0; i < count; i++)
		substring[i] = ch;
	substring[count] = 0;
	append(substring);
	return;
}

void MyString::append(const char* in_string, const int index, const int count) {
	if (!check_index(count)) return;
	if (!check_index(index) && index != 0) return;
	int new_m_len = count + m_len;

	char* new_string = NULL;
	unsigned int new_capacity = 0;
	if (new_m_len >= m_capacity) {
		new_capacity = value_capacity(new_m_len + 1);
		new_string = new char[new_capacity];
		for (unsigned int i = 0; i < m_len; i++)
			new_string[i] = m_data[i];
	}
	else new_string = m_data;

	unsigned int i = m_len;
	unsigned int last_idx = index + count + 1;
	for (unsigned int i_count = index; 
		i < new_m_len && i_count <= last_idx && in_string[i_count];
		i_count++, i++)
		new_string[i] = in_string[i_count];

	new_string[new_m_len] = 0;
	if (new_m_len >= m_capacity) {
		delete[] m_data;
		m_data = new_string;
		m_capacity = new_capacity;
	}
	m_len = new_m_len;
	return;
}

void MyString::append(const char* in_string) {
	int new_m_len = 0;
	for (new_m_len; in_string[new_m_len]; new_m_len++);
	append(in_string, 0, new_m_len);
	return;
}

void MyString::append(const std::string in_string) {
	return append(in_string.c_str());
}

void MyString::append(const std::string in_string, const int index, const int count) {
	return append(in_string.c_str(), index, count);
}

void MyString::replace(const int index, const int count, const char* in_string) {
	if (!check_index(index) && index != 0) return;
	if (!check_index(count)) return;
	for (unsigned int i_ad = 0, i = index; i_ad < count && i < m_len; i_ad++, i++)
		m_data[i] = in_string[i_ad];
	m_data[m_len] = 0;
	return;
}

void MyString::replace(const int index, const int count, std::string in_string) {
	return replace(index, count, in_string.c_str());
}

MyString MyString::substr(const int index) const{
	unsigned int new_m_len = m_len - index;
	return substr(index, new_m_len);
}

MyString MyString::substr(const int index, const int count) const {
	if (count < 0 || index < 0 || index >= m_len) {
		return MyString("");
	}
	unsigned int new_len = 0;
	if (count < m_len - index) 
		new_len = count;
	else
		new_len = m_len - index;
	
	MyString substr(m_data + index, new_len);
	return substr;
}

int MyString::find(const char* substring, const int index) const {
	if (index < 0) return -1;
	unsigned int i = 0;
	for (i = index; i < m_len; i++)
		if (m_data[i] == substring[0]) {
			int i_gen = i, i_sub = 0;
			for (i_gen, i_sub;
				substring[i_sub]
				&& m_data[i_gen]
				&& m_data[i_gen] == substring[i_sub];
				i_gen++, i_sub++);

			if (!substring[i_sub]) return i;
		}
	return -1;
}

int MyString::find(const std::string substring, const int index) const {
	return find(substring.c_str(), index);
}

MyString MyString::operator+ (const char* adding_string) const{
	MyString out(m_data);
	out.append(adding_string);
	return out;
}

MyString MyString::operator+(const std::string& adding_string) const{
	MyString out(m_data);
	out.append(adding_string.c_str());
	return out;
}

MyString MyString::operator+(const MyString& adding_string) const{
	MyString out(m_data);
	out.append(adding_string.c_str());
	return out;
}

MyString& MyString::operator+=(const char* adding_string) {
	this->append(adding_string);
	return *this;
}

MyString& MyString::operator+=(const std::string& adding_string) {
	this->append(adding_string);
	return *this;
}

MyString& MyString::operator= (const char* string_assignment) {
	clear();
	unsigned int new_len = 0;
	for (new_len; string_assignment[new_len]; new_len++);
	//if capacity is too small
	if (new_len >= m_capacity) {
		delete[] m_data;
		m_len = new_len;
		copy_string(string_assignment);
	}
	//if capacity is enough
	else {
		for (m_len = 0; m_len < new_len; m_len++)
			m_data[m_len] = string_assignment[m_len];
		m_data[m_len] = 0;
	}
	return *this;
}

MyString& MyString::operator=(const std::string& string_assignment) {
	return operator=(string_assignment.c_str());
}


MyString& MyString::operator=(const char letter_assignment) {
	char string_assignment[] = { letter_assignment, '\0' };
	return operator=(string_assignment);
}

char MyString::operator[](int index) const {
	if (index < m_len && index > -1)
		return m_data[index];
	else return 0;
}

int MyString::compare(const char* s2) const {
	unsigned int i = 0;
	for (i = 0; m_data[i] == s2[i] && s2[i] != 0 && m_data[i] != 0; i++);
	if (m_data[i] < s2[i]) return 1;
	if (m_data[i] == s2[i]) return 0;
	if (m_data[i] > s2[i]) return -1;
}

bool MyString::operator<(const char* comparing_string) const{
	return compare(comparing_string) > 0;
}

bool MyString::operator<(const char comparing_char) const {
	char comparing_string[] = { comparing_char, '\0' };
	return operator<(comparing_string);
}

bool MyString::operator<(const std::string& comparing_string) const {
	return operator<(comparing_string.c_str());
}

bool MyString::operator<(const MyString& comparing_string) const {
	return operator<(comparing_string.c_str());
}

bool MyString::operator<=(const char* comparing_string) const {
	return (compare(comparing_string) >= 0);
}

bool MyString::operator<=(const char comparing_char) const{
	char comparing_string[] = { comparing_char, '\0' };
	return operator<=(comparing_string);
}

bool MyString::operator<=(const std::string& comparing_string) const {
	return operator<=(comparing_string.c_str());
}

bool MyString::operator<=(const MyString& comparing_string) const {
	return operator<=(comparing_string.c_str());
}

bool MyString::operator>(const char* comparing_string) const {
	return compare(comparing_string) < 0;
}

bool MyString::operator>(const char comparing_char) const {
	char comparing_string[] = { comparing_char, '\0' };
	return operator>(comparing_string);
}

bool MyString::operator>(const std::string& comparing_string) const {
	return operator>(comparing_string.c_str());
}

bool MyString::operator>(const MyString& comparing_string) const {
	return operator>(comparing_string.c_str());
}

bool MyString::operator>=(const char* comparing_string) const  {
	return compare(comparing_string) <= 0;
}

bool MyString::operator>=(const char comparing_char) const{
	char comparing_string[] = { comparing_char, '\0' };
	return operator>=(comparing_string);
}

bool MyString::operator>=(const std::string& comparing_string) const {
	return operator>=(comparing_string.c_str());
}

bool MyString::operator>=(const MyString& comparing_string) const {
	return operator>=(comparing_string.c_str());
}

bool MyString::operator==(const char* comparing_string) const{
	return compare(comparing_string) == 0;
}

bool MyString::operator==(const char comparing_char) const{
	char comparing_string[] = { comparing_char, '\0' };
	return operator==(comparing_string);
}

bool MyString::operator==(const std::string& comparing_string) const{
	return operator==(comparing_string.c_str());
}

bool MyString::operator==(const MyString& comparing_string) const{
	return operator==(comparing_string.c_str());
}

bool MyString::operator!=(const char* comparing_string) const{
	return compare(comparing_string) != 0;
}

bool MyString::operator!=(const char comparing_char) const{
	char comparing_string[] = { comparing_char, '\0' };
	return operator!=(comparing_string);
}

bool MyString::operator!=(const std::string& comparing_string) const{
	return operator!=(comparing_string.c_str());
}

bool MyString::operator!=(const MyString& comparing_string) const{
	return operator!=(comparing_string.c_str());
}

std::istream& operator>> (std::istream& in, MyString& obj) {
	char ch_detector=0;
	in >> ch_detector;
	while (in.peek() != '\n' && ch_detector != 0) {
		obj.append(1, ch_detector);
		if (in.peek() == ' ')
			obj.append(1, ' ');
		in >> ch_detector;
	}
	obj.append(1, ch_detector);
	return in;
}

std::ostream& operator<< (std::ostream& out, MyString& obj) {
	out << obj.c_str();
	return out;
}