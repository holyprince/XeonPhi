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

__attribute__((target(mic))) void init(DATATYPE *x, int N1, int N2) {
#pragma omp parallel for num_threads(200)
	for (int i = 0; i < N1 * N2; i++) {
		x[i].real = i % 1000;
		x[i].imag = 0;
	}
}
__attribute__((target(mic))) void fasttranspose(int dimN,
		DATATYPE *inputdata) {

	int scale = dimN / 2;
#pragma omp parallel for num_threads(220)
	for (int i = 0; i < scale; i++) {
		//#pragma omp parallel for num_threads(20)
		for (int j = 0; j < dimN; j++) {
			double temp;
			int x, y;
			if (j < i) {
				x = i + scale;
				y = j + scale;

			} else {
				x = i;
				y = j;
			}
			temp = inputdata[y * dimN + x].real;
			inputdata[y * dimN + x].real = inputdata[x * dimN + y].real;
			inputdata[x * dimN + y].real = temp;
			temp = inputdata[y * dimN + x].imag;
			inputdata[y * dimN + x].imag = inputdata[x * dimN + y].imag;
			inputdata[x * dimN + y].imag = temp;

		}
	}
	/*
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
	 */
}
void printMatrix(int dimN, DATATYPE *inputdata) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
			printf("%f+i*%f ", inputdata[i * dimN + j].real,
					inputdata[i * dimN + j].imag);
		printf("\n");
	}
}
__attribute__((target(mic))) float testmodule2dfft(int dimx, int dimy) {

	int N1 = dimx, N2 = dimy;

	MKL_LONG N[2];
	N[0] = N1;
	N[1] = N2;

	DATATYPE *x = (DATATYPE*) mkl_malloc(N[0] * N[1] * sizeof(DATATYPE), ALIGN_SIZE);
	DATATYPE *out = (DATATYPE*) mkl_malloc(N[0] * N[1] * sizeof(DATATYPE), ALIGN_SIZE);

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
	struct timeval tv1, tv2;
	struct timezone tz;
	gettimeofday(&tv1, &tz);
	for (int i = 0; i < 100; i++) {

#pragma omp parallel for num_threads(235)
		for (int i = 0; i < dimone[0]; i++) {
			DftiComputeForward(dimonehandle, x + i * dimx, out + i * dimx);
		}
		//fasttranspose(N1, out);
#pragma omp parallel for num_threads(235)
		for (int i = 0; i < dimone[0]; i++) {
			DftiComputeForward(dimonehandle, out + i * dimx, x + i * dimx);
		}
		//fasttranspose(N1, x);
	}
	gettimeofday(&tv2, &tz);

	timeres = 1000 * (tv2.tv_sec - tv1.tv_sec)
			+ (tv2.tv_usec - tv1.tv_usec) / 1000;
	printf("FFT time %f ms \n", timeres);
	DftiFreeDescriptor(&dimonehandle);

	return timeres;

}

int main() {

	double res = 0;
#pragma offload target(mic:0)
	{
		for (int i = 0; i < 10; i++) {
			res += testmodule2dfft(2048, 2048);
		}
		printf("AVER %f \n", res / 10);
	}
	return 0;
}
