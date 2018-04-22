#include <iostream>

using namespace std;

class Cl {
	int a;
public:
	int get_a() {return a;}
};

class Cl1 : public Cl {
	int a = 10;
public:
	int get_a1() {return a;}
};

class Cl2 : public Cl {
	int a = 20;
};

int main() {
	Cl obj;
	cout << obj.get_a() << endl;
	Cl1 obj1;
	cout << obj1.get_a() << endl;
	cout << obj1.get_a1() << endl;
}

