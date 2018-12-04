#include <fstream>
#include <iostream>
using namespace std;

extern bool v;

void example_function1() {
	int i, j;
	int n = 1 + rand() % 100;

	double *a;

	a = new double[n];
	for (int i = 0; i < n; i++) {
			a[i] = 1 + rand() % 100;
	}

	for (i = 0; i < n - 1; i++) {
		for (j = 0; j < n - i - 1; j++) {
			if (a[j] > a[j + 1]) {
				int temp = a[j];
				a[j] = a[j + 1];
				a[j + 1] = temp;
			}
		}
	}

	if (v == true) {
		for (int i = 0; i < n; i++) {
			cout << a[i] << " ";
		}
	}
}

void error_func() {
	int delimoe = 5;
	int delitel = 5;
	int chastnoe = 0;
	while (true) {
		chastnoe = delimoe / (delitel-5);
	}
}

void example_function2() {
	ofstream fout("test.txt");
	for (int i = 0; i < 100; i++) {
		fout << "Пишу в файл " << endl; // запись строки в файл
	}
	fout.close();

	for (int i = 0; i < 100; i++) {
		fout << "Пишу на экран" << endl; // запись строки на экран
	}
	
}

void example_function3() {
	int n=1+rand()%20;
	
	double **a, **b,**c;
	
	a = new double*[n];
	for (int i = 0; i < n; i++){
		a[i] = new double[n];
		for (int j = 0; j < n; j++){
			a[i][j] = 1 + rand() % 20;
		}
	}

	b = new double*[n];
	for (int i = 0; i < n; i++) {
		b[i] = new double[n];
		for (int j = 0; j < n; j++) {
			b[i][j] = 1 + rand() % 20;
		}
	}

	c = new double*[n];
	for (int i = 0; i<n; i++)
	{
		c[i] = new double[n];
		for (int j = 0; j<n; j++)
		{
			c[i][j] = 0;
			for (int k = 0; k<n; k++)
				c[i][j] += a[i][k] * b[k][j];
		}
	}
	
	if (v == true) {
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
				cout << c[i][j] << " ";
			cout << endl;
		}
	}
}

