// ie531CA2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <random>
#include <chrono>
#include <string.h>
#include <math.h>
#include "include.h"
#include "newmat.h"
#include "newmatio.h"
#include "newmatap.h"

#define _CRT_SECURE_NO_WARNINGS

typedef std::pair<double, double> Box_Muller_Pair;

using namespace std;

// cf http://www.cplusplus.com/reference/random/uniform_real_distribution/operator()/
// If you want to set a seed -- do it only after debug phase is completed
// otherwise errors will not be repeatable.
unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
default_random_engine generator(seed);

// U.I.I.D. RV generator
double get_uniform()
{
	std::uniform_real_distribution <double> distribution(0.0, 1.0);
	double number = distribution(generator);
	return (number);
}

// Using the Gaussian generator that is part of the C++ STL
double get_gaussian(double mean, double standard_deviation)
{
	std::normal_distribution<double> distribution(mean, standard_deviation);
	double number = distribution(generator);
	return (number);
}

Matrix Generate_d_Dim_Vector(int d, int k) {
	Matrix u(d,1);
	Real * holdermatrix;
	holdermatrix = new Real[d];
	// Each entry in row can be random, then every component ui in the row is scaled such that ui = ui * sqrt(d/k)/norm(u)
	for (int i = 0;i < d;i++) {
		holdermatrix[i] = get_gaussian(0, 1/sqrt(k));
	}
	u << holdermatrix;
	return u;
}

Matrix Generate_Row(int k,int d)
{
	Matrix u(1,d);
	double sum = 0;
	double holder = 0;
	double * holdermatrix;
	holdermatrix = new double[d];
	// Each entry in row can be random, then every component ui in the row is scaled such that ui = ui * sqrt(d/k)/norm(u)
	for (int i = 0;i <= d;i++) {
		holder = get_gaussian(0, 1);
		holdermatrix[i] = holder;
		sum += holder*holder;
	}
	u << holdermatrix;


	for (int i = 1;i <= d;i++) {
		u(1, i) = sqrt(d / k)*u(1, i) / sqrt(sum);

	}
	return u;
	
}

Matrix Generate_Random_Projection(int k, int d)
{
	// write code here that follows what we discussed in class
	// Make k many rows, each of dimension d

	Matrix A(k, d);
	Matrix u(1, d);


	for (int i = 1;i <= k;i++) {
		u=Generate_Row(k,d);
		A.row(i) << u;
	}

	return A;
}

int main(int argc, const char * argv[])
{
	int original_dimension, reduced_dimension, no_of_cols, no_of_trials;
	double epsilon, delta, diff;
	clock_t time_before, time_after;

	sscanf(argv[1], "%d", &original_dimension);
	sscanf(argv[2], "%d", &reduced_dimension);
	sscanf(argv[3], "%d", &no_of_cols);
	sscanf(argv[4], "%lf", &epsilon);
	sscanf(argv[5], "%lf", &delta);
	ifstream input_file(argv[6]);
	sscanf(argv[7], "%d", &no_of_trials);

	// Hard Coded for test
	//original_dimension = 10000;
	//reduced_dimension = 200;
	//no_of_cols = 1000;
	//epsilon = 0.1;
	//delta=0.1;
	//ifstream input_file("JL_Data1");
	//no_of_trials = 10000;

	cout << "Johnson-Lindenstrauss Lemma Demo" << endl;
	cout << "Reading a (" << original_dimension << " x " << no_of_cols << ") Matrix from file '";
	cout << argv[6] << "'" << endl;
	cout << "Reduced Dimension = " << reduced_dimension << endl;
	cout << "epsilon = " << epsilon << endl;
	cout << "delta = " << delta << endl;
	cout << "Reduced Dimension (i.e. " << reduced_dimension << ") should be >= " << ceil(1 / (epsilon*epsilon)*log(((double)no_of_cols) / delta));
	cout << " for the bound to hold with probability " << 1 - delta << endl;

	Matrix R = Generate_Random_Projection(reduced_dimension, original_dimension);

	Matrix A(original_dimension, no_of_cols);

	time_before = clock(); // recording time before we started to read data
	std::vector<double> temp;
	string num;
	while (getline(input_file,num,',')) {
		cout << num << endl;
		temp.push_back(stod(num));
	}

	for (int j = 1; j <= original_dimension; j++)
	{
		for (int i = 1; i <= no_of_cols; i++)
		{
			//cout << x << endl;
			A(j, i) = temp[i+j-2];
		}
	}
	cout << "A(1,1) is " << A(1,1) << endl;
	cout << "A(1,2) is " << A(1, 2) << endl;
	cout << "temp(1) is " << temp[0] << endl;
	cout << "temp(2) is " << temp[1] << endl;
	time_after = clock(); // recording time after testing is complete
	diff = ((double)time_after - (double)time_before);
	cout << "It took " << (diff / CLOCKS_PER_SEC) / 60.0 << " minutes to read data from file '" << argv[6] << "'" << endl;
	// testing Johnson-Lindenstrauss Lemma
	int no_of_hits = 0;
	cout << "#Trails for the testing-phase = " << no_of_trials << endl;
	// this is the reduced-dimension representation of the x's (i.e the matrix of y's)
	Matrix C(reduced_dimension, original_dimension);
	C = R * A;

	time_before = clock(); // recording time before the testing starts

						   // write code here for verification of JL-Lemma
	for(int i=0;i<no_of_trials;i++){
		Matrix x1 = Generate_d_Dim_Vector(original_dimension, reduced_dimension);
		Matrix x2 = Generate_d_Dim_Vector(original_dimension, reduced_dimension);
		Matrix y1 (reduced_dimension,1);
		Matrix y2 (reduced_dimension,1);
		y1 = R*x1;
		y2 = R*x2;
		//cout << "x1(1,1) is " << x1(1, 1) << endl;
		//cout << "x2(1,1) is " << x2(1, 1) << endl;
		//cout << "y1(1,1) is " << y1(1,1) << endl;
		//cout << "y2(1,1) is " << y2(1,1) << endl;
		cout << "Lower Bound " << (1 - epsilon)*sqrt(DotProduct((x1 - x2), (x1 - x2))) << endl;
		cout << "Distance in d-Dimensions " << sqrt(DotProduct((y1 - y2), (y1 - y2))) << endl;
		cout << "Upper Bound " << (1 + epsilon)*sqrt(DotProduct((x1 - x2), (x1 - x2))) << endl;

		if ((1 - epsilon)*sqrt(DotProduct((x1 - x2),(x1-x2))) <= sqrt(DotProduct((y1 - y2), (y1 - y2))) && (1 + epsilon)*sqrt(DotProduct((x1 - x2), (x1 - x2))) >= sqrt(DotProduct((y1 - y2), (y1 - y2)))) {
			no_of_hits++;
		}
	}


	time_after = clock(); // recording time after testing is complete
	diff = ((double)time_after - (double)time_before);
	cout << "It took " << (diff / CLOCKS_PER_SEC) / 60.0 << " minutes for testing to be completed" << endl;

	cout << "Johnson-Lindenstrauss Lemma is satisfied " << no_of_hits << "-many times over ";
	cout << no_of_trials << " attempts" << endl;
	cout << "Empirical Probability = " << ((double)no_of_hits / no_of_trials);

	return 0;
}

