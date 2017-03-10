//MIC version
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "mkl_service.h"
#include "mkl_dfti.h"
#include "mkl.h"
#include<sys/time.h>
#include "omp.h"
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
	//omp_set_nested(true);
#pragma omp parallel for num_threads(235)
	for (int i = 0; i < scale; i++) {
		for (int j = 0; j < dimN; j++) {
			//float temp;
			int x, y;
			int xdim, ydim;
			if (j < i) {
				x = i + scale;
				y = j + scale;

			} else {
				x = i;
				y = j;
			}
			float temp;
			xdim = x * dimN + y;
			ydim = y * dimN + x;
			/*		inputdata[xdim].real=inputdata[xdim].real+inputdata[ydim].real;
			 inputdata[ydim].real=inputdata[xdim].real-inputdata[ydim].real;
			 inputdata[xdim].real=inputdata[xdim].real-inputdata[ydim].real;
			 inputdata[xdim].imag=inputdata[xdim].imag+inputdata[ydim].imag;
			 inputdata[ydim].imag=inputdata[xdim].imag-inputdata[ydim].imag;
			 inputdata[xdim].imag=inputdata[xdim].imag-inputdata[ydim].imag;*/

			temp = inputdata[ydim].real;
			inputdata[ydim].real = inputdata[xdim].real;
			inputdata[xdim].real = temp;
			temp = inputdata[ydim].imag;
			inputdata[ydim].imag = inputdata[xdim].imag;
			inputdata[xdim].imag = temp;

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
__attribute__((target(mic))) void fasttranspose2(int dimN,
DATATYPE *inputdata) {

#pragma omp parallel for num_threads(235)
#pragma ivdep
	//#pragma simd
	for (int i = 0; i < dimN; i++) {
		//#pragma omp parallel for num_threads(20)
#pragma simd
		for (int j = i + 1; j < dimN; j++) {
			//float temp;

			int xdim, ydim;

			float temp;
			xdim = i * dimN + j;
			ydim = j * dimN + i;
			temp = inputdata[ydim].real;
			inputdata[ydim].real = inputdata[xdim].real;
			inputdata[xdim].real = temp;
			temp = inputdata[ydim].imag;
			inputdata[ydim].imag = inputdata[xdim].imag;
			inputdata[xdim].imag = temp;

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
__attribute__((target(mic))) float testmodule2dfft(int dimx, int dimy) {

	int N1 = dimx, N2 = dimy;

	MKL_LONG N[2];
	N[0] = N1;
	N[1] = N2;

	DATATYPE *x = (DATATYPE*) mkl_malloc(N[0] * N[1] * sizeof(DATATYPE),
			ALIGN_SIZE);
	DATATYPE *out = (DATATYPE*) mkl_malloc(N[0] * N[1] * sizeof(DATATYPE),
			ALIGN_SIZE);

	double timeres;

	MKL_LONG dimone[1];
	dimone[0] = N1;
	MKL_LONG status = 0;
	DFTI_DESCRIPTOR_HANDLE dimonehandle = 0;
	DftiCreateDescriptor(&dimonehandle, DFTI_SINGLE, DFTI_COMPLEX, 1,
			dimone[0]);
	DftiSetValue(dimonehandle, DFTI_PLACEMENT, DFTI_NOT_INPLACE);
	status = DftiCommitDescriptor(dimonehandle);
	//////////////////////////////////////////////////////////////////////////////////////
	MKL_LONG stride[2];
	DFTI_DESCRIPTOR_HANDLE desc_handle_dim1;
	status = DftiCreateDescriptor(&desc_handle_dim1, DFTI_SINGLE, DFTI_COMPLEX,
			1, dimx);
	stride[0] = 0;
	stride[1] = dimy;
	status = DftiSetValue(desc_handle_dim1, DFTI_NUMBER_OF_TRANSFORMS, 1);
	status = DftiSetValue(desc_handle_dim1, DFTI_INPUT_DISTANCE, 1);
	status = DftiSetValue(desc_handle_dim1, DFTI_OUTPUT_DISTANCE, 1);
	status = DftiSetValue(desc_handle_dim1, DFTI_INPUT_STRIDES, stride);
	status = DftiSetValue(desc_handle_dim1, DFTI_OUTPUT_STRIDES, stride);
	status = DftiCommitDescriptor(desc_handle_dim1);
	init(x, N[0], N[1]);
	struct timeval tv1, tv2;
	struct timezone tz;
	gettimeofday(&tv1, &tz);
	for (int i = 0; i < 1; i++) {

#pragma omp parallel for num_threads(200)
		for (int j = 0; j < dimone[0]; j++) {
			DftiComputeForward(dimonehandle, x + j * dimx, out + j * dimx);
		}
		//printMatrix(dimx,x);
#pragma omp parallel for num_threads(200)
		for (int j = 0; j < dimone[0]; j++) {
			status = DftiComputeForward(desc_handle_dim1, out + j);
		}

		/*		fasttranspose2(N1, out);
		 //printMatrix(dimx,x);
		 #pragma omp parallel for num_threads(235)
		 for (int j = 0; j < dimone[0]; j++) {
		 DftiComputeForward(dimonehandle, out + j * dimx, out + j * dimx);
		 }*/
		//fasttranspose2(N1, out);
		//printMatrix(dimx,x);
	}
	gettimeofday(&tv2, &tz);

	timeres = 1000 * (tv2.tv_sec - tv1.tv_sec)
			+ (tv2.tv_usec - tv1.tv_usec) / 1000;
	printf("FFT time %f ms \n", timeres);
	DftiFreeDescriptor(&dimonehandle);
	DftiFreeDescriptor(&desc_handle_dim1);

	return timeres;

}

int main() {

	double res = 0;
#pragma offload target(mic:0)
	{
		for (int i = 0; i < 1; i++) {
			res += testmodule2dfft(8192, 8192);
		}
		printf("AVER %f \n", res / 10);
	}
	return 0;
}
