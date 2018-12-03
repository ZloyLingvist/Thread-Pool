#include <iostream>
#include <fstream>
using namespace std;

void example_function1() {
	int res = 0;
	while(true){
		res = res + 100;
		if (res % 2 == 0){
			res = res / 2;
		}
		if (res > 100000){
			break;
		}
	}

	std::cout << res << std::endl;
}

void example_function2() {
	ofstream fout("test.txt");
	for (int i = 0; i < 200; i++) {
		fout << "Пишу в файл" << endl; // запись строки в файл
	}
	fout.close();
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
	
	for (int i = 0; i<n; i++)
	{
		for (int j = 0; j<n; j++)
			cout << c[i][j] << " ";
		cout << endl;
	}
	

}

