// ie531ca3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <random>
#include <chrono>
//#include <string.h>
#include <math.h>
#include "include.h"
#include "newmat.h"
#include "newmatio.h"
#include "newmatap.h"

#define _CRT_SECURE_NO_WARNINGS

using namespace std;

// cf http://www.cplusplus.com/reference/random/uniform_real_distribution/operator()/
// If you want to set a seed -- do it only after debug phase is completed
// otherwise errors will not be repeatable.
unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
default_random_engine generator(seed);

// U.I.I.D. RV generator
double get_uniform()
{
	std::uniform_real_distribution <double> distribution(-5.0, 5.0);
	double number = distribution(generator);
	return (number);
}

Matrix repeated_squaring(Matrix B, int exponent, int no_rows) {
	Matrix I(no_rows, no_rows);
	for (int i = 1;i <= no_rows;i++) {
		for (int j = 1; j <= no_rows;j++) {
			I(i, j) = 1;
		}
	}
	if (exponent == 0) {
		return I;
	}
	if (exponent == 1) {
		return B;
	}
	else if (exponent % 2 == 1) {
		return(B*repeated_squaring(B*B,(exponent-1)/2,no_rows));
	}
	else if (exponent % 2 == 0) {
		return(repeated_squaring(B*B, exponent/ 2, no_rows));
	}

}

//Matrix direct_multiplication(Matrix B, int exponent) {
//	
//	if (exponent == 1) {
//		return B;
//	}
//	else {
//		return direct_multiplication(B*B, exponent - 1);
//	}
//}

Matrix direct_multiplication(Matrix B, int exponent, int no_rows) {
	Matrix Bnew(no_rows, no_rows);
	Bnew = B;
	for (int i = 1;i < exponent;i++) {
		Bnew = Bnew * B;
	}

	return Bnew;
}

int main(int argc, const char * argv[])
{
	int matrix_dimension, exponent;
	double diff;
	clock_t time_before_repeated_squares, time_after_repeated_squares, time_before_brute_force, time_after_brute_force;

	sscanf(argv[1], "%d", &exponent);
	sscanf(argv[2], "%d", &matrix_dimension);


	cout << "Number of rows/columns in the square matrix is: " << matrix_dimension << endl;
	cout << "The exponent is: " << exponent << endl;

	Matrix B(matrix_dimension, matrix_dimension);

	for (int i = 1;i <= matrix_dimension;i++) {
		for (int j = 1;j <= matrix_dimension;j++) {
			B(i,j) = get_uniform();
		}

	}

	//cout << "" << endl << "=============================================================== " << endl;
	//cout << "The original random matrix: " << endl;
	//for (int i = 1;i <= matrix_dimension;i++) {
	//	for (int j = 1; j <= matrix_dimension; j++) {
	//		cout << B(i, j) << " " ;
	//	}
	//	cout << " " << endl;
	//}
	//cout << "" << "=============================================================== " << endl << endl;

	time_before_repeated_squares = clock();
	// Call repeated squares algorithm
	Matrix Bnew(matrix_dimension, matrix_dimension);
	Bnew = repeated_squaring(B, exponent, matrix_dimension);
	time_after_repeated_squares = clock();

	diff = ((double)time_after_repeated_squares - (double)time_before_repeated_squares);
	cout << "It took " << (diff / CLOCKS_PER_SEC) << " seconds to complete Repeated Squares" << endl;

	cout << "The multiplied Matrix using the Repeated Squares " << endl;
	cout << "" << endl << "=============================================================== " << endl;
	for (int i = 1;i <= matrix_dimension;i++) {
		for (int j = 1; j <= matrix_dimension; j++) {
			cout << Bnew(i, j) << " ";
		}
		cout << endl;
	}
	cout << "" << "=============================================================== "<<endl;

	Matrix Bdirectmult(matrix_dimension, matrix_dimension);

	time_before_brute_force = clock();
	Bdirectmult = direct_multiplication(B, exponent,matrix_dimension);
	time_after_brute_force = clock();

	cout << "The multiplied Matrix using Direct Multiplication " << endl;
	cout << "" << endl << "=============================================================== " << endl;
	for (int i = 1;i <= matrix_dimension;i++) {
		for (int j = 1; j <= matrix_dimension; j++) {
			cout << Bdirectmult(i, j) << " ";
		}
		cout << endl;
	}
	cout << "" << "=============================================================== " << endl;

	diff = ((double)time_after_brute_force - (double)time_before_brute_force);
	cout << "It took " << (diff / CLOCKS_PER_SEC) << " seconds to complete Brute Force" << endl;

	// Now we want to get some points to plot. We will use a 100x100 matrix and store the times as they vary according to the 
	int new_exponent = 0;
	int repeated_squares_times[10];
	int direct_multiplication_times[10];
	int x_axis[10] = {100,200,300,400,500,600,700,800,900,1000};

	cout << "List of times for repeated squares" << endl;
	for (int i = 1;i <= 10;i++) {
		new_exponent = 100 * i;
		time_before_repeated_squares = clock();
		Matrix Bnew(matrix_dimension, matrix_dimension);
		Bnew = repeated_squaring(B, new_exponent, matrix_dimension);
		time_after_repeated_squares = clock();

		diff = ((double)time_after_repeated_squares - (double)time_before_repeated_squares);
		repeated_squares_times[i-1] = diff;
		cout << diff << " " << endl;
	}
	cout << endl;

	cout << "List of times for direct multiplication" << endl;
	for (int i = 1;i <= 10;i++) {
		new_exponent = 100 * i;
		time_before_repeated_squares = clock();
		Matrix Bnew(matrix_dimension, matrix_dimension);
		Bnew = direct_multiplication(B, new_exponent, matrix_dimension);
		time_after_repeated_squares = clock();

		diff = ((double)time_after_repeated_squares - (double)time_before_repeated_squares);
		direct_multiplication_times[i-1] = diff;
		cout << diff << " " << endl;
	}

	ofstream myfile;
	myfile.open("output.txt");
	for (int i = 0;i < 10;i++) {
		myfile << x_axis[i] << "," << repeated_squares_times[i] << "," << direct_multiplication_times[i] <<"\n";
	}
	myfile.close();

    return 0;
}

