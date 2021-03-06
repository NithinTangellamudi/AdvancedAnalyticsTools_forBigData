//
//  main.cpp
//  Matrix Sketch
//
//
//  Edo Liberty's paper is about using a streaming algorithm for
//  Matrix Sketching.  But we are going to implement/test this in
//  in a non-streaming context.  "A" is a large (n x m) data-matrix
//  where m >>> n. We want to find a smaller (n x k) "B" matrix, where
//  k = 2/epsilon, such that the Frobenius-Norm of (A*A.t() - B*B.t())
//  is <= epsilon * the Frobenius-Norm of (A*A.t())

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <math.h>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include "include.h"
#include "newmat.h"
#include "newmatio.h"
#include "newmatap.h"

using namespace std;

double Frobenious_Norm(Matrix Data)
{
	double sum = 0;
	for (int i = 1;i <= Data.nrows();i++) {
		for (int j = 1;j <= Data.ncols();j++) {
			sum += Data(i, j)*Data(i, j);
		}
	}
	return sqrt(sum);
}

Matrix Matrix_Sketch(Matrix Data, double epsilon)
{
	// Edo Liberty's Matrix Sketch will have the same number of rows
	// as the original data; the #cols is ceil(2.0/epsilon)
	int cols_of_sketch = ceil(2.0 / epsilon);

	if (cols_of_sketch < Data.nrows())
	{
		Matrix Result(Data.nrows(), cols_of_sketch);
		// write this part of the code yourself
		// initially put first cols_of_sketch number of coulumns into the Result matrix
		for (int i = 1;i <= cols_of_sketch;i++) {
			Result.Column(i) = Data.Column(i);
		}

		for (int i = cols_of_sketch+1;i <= Data.ncols();i++) {
			Matrix U(Data.nrows(), cols_of_sketch);
			DiagonalMatrix D(cols_of_sketch);
			Matrix V(cols_of_sketch, cols_of_sketch);
			IdentityMatrix I(cols_of_sketch);

			SVD(Result, D, U, V);

			DiagonalMatrix Dnew(cols_of_sketch);
			Dnew = D * D - I * D(cols_of_sketch)*D(cols_of_sketch);

			for (int i = 1;i <= cols_of_sketch;i++) {
				Dnew(i) = sqrt(Dnew(i));
			}
			if (i != Data.ncols()) {
				Result = U * Dnew;
				Result.Column(cols_of_sketch) = Data.Column(i);
			}
		}


		return Result;
	}
	else
	{
		// When matrix is "fat"


		Matrix Result(Data.nrows(), Data.nrows());
		// write this part of the code yourself


		// initially put first cols_of_sketch number of coulumns into the Result matrix
		for (int i = 1;i <= Data.nrows();i++) {
			Result.Column(i) = Data.Column(i);
		}

		for (int i = Data.nrows() + 1;i <= Data.ncols();i++) {
			Matrix U(Data.nrows(), Data.nrows());
			DiagonalMatrix D(Data.nrows());
			Matrix V(Data.nrows(), Data.nrows());
			IdentityMatrix I(Data.nrows());

			SVD(Result, D, U, V);

			DiagonalMatrix Dnew(Data.nrows());
			Dnew = D * D - I * D(Data.nrows())*D(Data.nrows());

			for (int i = 1;i <= Data.nrows();i++) {
				Dnew(i) = sqrt(Dnew(i));
			}
			if (i != Data.ncols()) {
				Result = U * Dnew;
				Result.Column(Data.nrows()) = Data.Column(i);
			}
		}


		return Result;
	}

}

int main(int argc, char* argv[])
{
	int dimension, no_of_data_points;
	double epsilon;

	sscanf(argv[1], "%d", &dimension);
	sscanf(argv[2], "%d", &no_of_data_points);
	sscanf(argv[3], "%lf", &epsilon);
	ifstream input_file(argv[4]);
	ofstream output_file(argv[5]);

	Matrix Data(dimension, no_of_data_points);

	cout << "Edo Liberty's Matrix Sketching Algorithm" << endl;
	cout << "----------------------------------------" << endl;
	cout << "Original Data-Matrix has " << dimension << "-rows & " << no_of_data_points << "-cols" << endl;
	cout << "Epsilon = " << epsilon << " (i.e. max. of " << 100 * epsilon << "% reduction of  Frobenius-Norm of the Sketch Matrix)" << endl;
	cout << "Input File = " << argv[4] << endl;

	// Read the Data
	for (int i = 1; i <= dimension; i++)
		for (int j = 1; j <= no_of_data_points; j++)
		{
			double x;
			input_file >> x;
			Data(i, j) = x;
		}
	// Do SVD of the data
	// Data is n x m with m>>n
	// DataT is m x n with m>>n

	// Compute the Frobenius-Norm of the original Data-Matrix
	Matrix DataFlipCoin(dimension, dimension);
	DataFlipCoin = Data * Data.t();
	double Data_Forbenius_Norm = Frobenious_Norm(DataFlipCoin);
	cout << "Frobenius Norm of the (" << DataFlipCoin.nrows() << " x " << Data.ncols() << ") Data Matrix = ";
	cout << Data_Forbenius_Norm << endl;

	Matrix Sketchtemp(dimension, min(dimension, (int)ceil(2 / epsilon)));

	// Finesse

	Sketchtemp = Matrix_Sketch(Data, epsilon);

	Matrix Sketch(dimension, dimension);
	Sketch = Sketchtemp * Sketchtemp.t();

	double Sketch_Forbenius_Norm = Frobenious_Norm(Sketch);
	cout << "Frobenius Norm of the (" << Sketch.nrows() << " x " << Sketchtemp.ncols() << ") Sketch Matrix = ";
	cout << Sketch_Forbenius_Norm << endl;
	cout << "Change in Frobenius-Norm between Sketch & Original  = ";
	cout << setprecision(3) << 100 * (Sketch_Forbenius_Norm - Data_Forbenius_Norm) / Data_Forbenius_Norm << "%" << endl;

	output_file << Sketch;
	cout << "File `" << argv[5] << "' contains a (" << Sketch.nrows() << " x " << Sketch.ncols();
	cout << ") Matrix-Sketch" << endl;
}