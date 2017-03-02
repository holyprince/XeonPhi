#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "mkl_service.h"
#include "mkl_dfti.h"
#include "mkl.h"
#include<sys/time.h>
//#define PRINT

#define ALIGN_SIZE 64
#define REP_TIMES 100
#define DATATYPE MKL_Complex8

void init(DATATYPE *x, int N1, int N2) {
	for (int i = 0; i < N1 * N2; i++) {
		x[i].real = i % 1000;
		x[i].imag = 0;
	}
}
void fasttranspose(int dimN, DATATYPE *inputdata) {
	double temp;
	for (int i = 0; i < dimN; i++) {
		for (int j = i; j < dimN; j++) {
			temp = inputdata[j * dimN + i].real;
			inputdata[j * dimN + i].real = inputdata[i * dimN + j].real;
			inputdata[i * dimN + j].real = temp;
			temp = inputdata[j * dimN + i].imag;
			inputdata[j * dimN + i].imag = inputdata[i * dimN + j].imag;
			inputdata[i * dimN + j].imag = temp;
		}
	}
}
void printMatrix(int dimN, DATATYPE *inputdata) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
			printf("%f+i*%f ", inputdata[i * dimN + j].real,
					inputdata[i * dimN + j].imag);
		printf("\n");
	}
}
float testmodule2dfft(int dimx, int dimy) {

	int N1 = dimx, N2 = dimy;

	MKL_LONG N[2];
	N[0] = N1;
	N[1] = N2;

	DATATYPE *x = (DATATYPE*) mkl_malloc(N[0] * N[1] * sizeof(DATATYPE),
			ALIGN_SIZE);
	DATATYPE *out = (DATATYPE*) mkl_malloc(N[0] * N[1] * sizeof(DATATYPE),
			ALIGN_SIZE);

	init(x, N[0], N[1]);

	double timeres;

	MKL_LONG dimone[1];
	dimone[0] = N1;
	MKL_LONG status = 0;
	DFTI_DESCRIPTOR_HANDLE dimonehandle = 0;
	DftiCreateDescriptor(&dimonehandle, DFTI_SINGLE, DFTI_COMPLEX, 1,
			dimone[0]);
	DftiSetValue(dimonehandle, DFTI_PLACEMENT, DFTI_NOT_INPLACE);
	status = DftiCommitDescriptor(dimonehandle);

	for (int i = 0; i < dimone[0]; i++) {
		DftiComputeForward(dimonehandle, x + i * dimx, out + i * dimx);
	}
	printf("First res\n");
	printMatrix(dimx, out);
	printf("\n");
	fasttranspose(N1, out);
	printf("First res after transpose\n");

	printMatrix(dimx, out);
	printf("\n");


	for (int i = 0; i < dimone[0]; i++) {
		DftiComputeForward(dimonehandle, out + i * dimx, x + i * dimx);
	}
	printf("Second res\n");
	printMatrix(dimx, x);
	printf("\n");
	fasttranspose(N1, x);
	printf("Final res\n");
	printMatrix(dimx, x);
	printf("\n");
	DftiFreeDescriptor(&dimonehandle);

	printf("\n");

	return timeres;

}

int main() {

	testmodule2dfft(8192, 8192);
	return 0;
}

