#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <fstream>

#include "table.hpp"

using namespace std;

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
