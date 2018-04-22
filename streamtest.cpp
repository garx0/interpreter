#include <iostream>

using namespace std;





class Scanner {
	istream& input;
	//...
public:
	explicit Scanner(istream& a_input): input(a_input) {}
	//...
};









int main() {
	/*
	char c[3] = {'1', '1', '1'};
	for(int i = 0; i < 3; i++) {
		//c = '1';
		cin.get(c[i]);
		cout << "good = " << cin.good() << endl;
		cout << "eof = " << cin.eof() << endl;
		cout << "c = " << c[i] << " = #" << (int) c[i] << endl;	
		cout << endl;
	}
	*/
	char c;
	cin.get(c);
	string str1 = {c};
	cout << str1;
	string str2 = {c, 0};
	cout << str2;
}

//если дали хотя бы один символ в поток:
//    читает и на след. get(c) ждет дальше (good всегда = 1)
//если дали 0 символов на очередном "ожидании" (т.е. сразу ^D), то:
//    не ждет больше, good = 0, get не меняет c
