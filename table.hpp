#pragma once

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <fstream>

using namespace std;

template<class T>
class Table {
	//элементы нумеруются с 1
	//в векторе нулевой элемент - "пустой" (T())
	//при инициализации таблицы этот пустой элемент указывать не нужно,
	//	начинать сразу с первого.
	vector<T> contents;
public:
	Table(): contents{ T() } {}
	Table(initializer_list<T> list);
	//список: {элементы, начиная с 1-го}
	int size() const;
	int put(const T& elem);
	//если такого элемента нет в таблице, он заносится в таблицу;
	//в любом случае, возвращается номер элемента в таблице
	int find(const T& elem) const;
	//возвращает номер элемента в таблице;
	//если не найден, то 0
	int push_back(const T& elem);
	//вставляет в конец элемент и возвращает его номер
	const T& operator[](int ind) const;
	T& operator[](int ind);
	vector<T> getVector() const;
	//выдает вектор, где нулевой элемент пустой, остальные элементы
	//	(с 1-го) - элементы таблицы
};

template<class T>
Table<T>::Table(initializer_list<T> list)
{
	contents.push_back( T() );
	for(auto& item : list) {
		contents.push_back(item);
	}
}

template<class T>
int Table<T>::size() const {
	return contents.size() - 1;
}

template<class T>
int Table<T>::put(const T& elem)
{
	typename vector<T>::const_iterator res = 
		std::find(contents.begin() + 1, contents.end(), elem);
	int num = res - contents.begin();
	if(res == contents.end() ) {
		contents.push_back(elem);
	}
	return num;
}

template<class T>
int Table<T>::find(const T& elem) const
{
	typename vector<T>::const_iterator res = std::find(contents.begin() + 1, contents.end(), elem);
	int num = res != contents.end() ? res - contents.begin() : 0;
	return num;
}

template<class T>
int Table<T>::push_back(const T& elem) {
	contents.push_back(elem);
	return size();
}

template<class T>
const T& Table<T>::operator[](int ind) const {
	return contents[ind];
}	

template<class T>
T& Table<T>::operator[](int ind) {
	return contents[ind];
}	

template<class T>
vector<T> Table<T>::getVector() const {return contents;}
