#ifndef _MY_LIB_STRING_H_
#define _MY_LIB_STRING_H_

#include<iostream>
#include <list>


class MyString {
public:
	const int BLOCK_SIZE = 15;
	//Constructors and Destructor
	MyString();
	MyString(const std::initializer_list<char>& input_data);
	MyString(const char* input_data);
	MyString(const char* input_data, const int count);
	MyString(const MyString& input_data);
	MyString(const std::string& input_data);
	MyString(const int, const char);
	~MyString();
	//Operators
	//Plus
	MyString operator+ (const char*) const;
	MyString operator+ (const std::string&) const;
	MyString operator+ (const MyString&) const;
	MyString& operator+= (const char*);
	MyString& operator+= (const std::string&);
	//equally
	virtual MyString& operator= (const char*);
	virtual MyString& operator= (const std::string&);
	virtual MyString& operator= (const char);
	char operator[](const int) const;
	//compare
	bool operator> (const char*) const;
	bool operator> (const std::string&) const;
	bool operator> (const char)const;
	bool operator> (const MyString&)const;
	bool operator< (const char*)const;
	bool operator< (const std::string&)const;
	bool operator< (const char)const;
	bool operator< (const MyString&)const;
	bool operator>= (const char*)const;
	bool operator>= (const std::string&)const;
	bool operator>= (const char)const;
	bool operator>= (const MyString&)const;
	bool operator<= (const char*)const;
	bool operator<= (const std::string&)const;
	bool operator<= (const char)const;
	bool operator<= (const MyString&)const;
	bool operator== (const char*)const;
	bool operator== (const std::string&)const;
	bool operator== (const char)const;
	bool operator== (const MyString&)const;
	bool operator!= (const char*)const;
	bool operator!= (const std::string&)const;
	bool operator!= (const char)const;
	bool operator!= (const MyString&)const;

	friend std::ostream& operator<<(std::ostream&, MyString&);
	friend std::istream& operator>> (std::istream&, MyString&);

	//Capacity
	virtual int lenght() const;
	virtual char* c_str() const;
	virtual char* data() const;
	virtual int size() const;
	virtual bool empty() const;
	virtual int capacity() const;
	virtual void clear();
	virtual void shrink_to_fit();

	virtual void insert(int index, int count, char letter);
	void insert(int index, const char* subline);
	void insert(int index, const char* subline, int count);
	virtual void insert(const int index, const std::string& new_string);
	virtual void insert(const int index, const std::string& new_string, int count);
	virtual void erase(const int index, const int count);
	virtual void append(const int count, const char ch);
	void append(const char* in_string, const int index, const int count);
	void append(const char* in_string);
	virtual void append(const std::string);
	virtual void append(const std::string, const int index, const int count);
	void replace(const int index, const int count, const char* in_string);
	virtual void replace(const int index, const int count, std::string);
	virtual MyString substr(const int index) const;
	virtual MyString substr(const int index, const int count) const;
	int find(const char* substring, const int index = 0) const;
	virtual int find(const std::string substring, const int index = 0) const;
private:
	int compare(const char*) const;
	bool check_index(int value);
	int value_capacity(int len);
	void copy_string(const char* input_data);
protected:
	char* m_data;
	int m_len;
	int m_capacity;
};


#endif