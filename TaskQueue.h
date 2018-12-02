#include <iostream>
using namespace std;

void example_function1() {
	int res = 0;
	for (int i = 0; i < 100000; i++) {
		for (int j = i; j > 50000; j--) {
			res = res - j;
		}
	}
	std::cout << res << std::endl;
}

void example_function2() {
	int res = 0;
	for (int i = 0; i < 100000; i++) {
		for (int j = i; j > 50000; j--) {
			res = res + j;
		}
	}
	std::cout << res << std::endl;
}

