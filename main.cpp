#include "expression.h"
#include <iostream>
#include <stdexcept>
using namespace std;

int main() {
	try {
		expression_namespace::Expression e1("(3+2)/(7-5)");
		cout << e1.compute();
	}
	catch (exception e) {
		cout << e.what() << endl;
	}
	return 0;
}
