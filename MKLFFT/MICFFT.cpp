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
void init(MKL_Complex8 *x, int N1, int N2) {
	for (int i = 0; i < N1 * N2; i++) {
		x[i].real = i % 1000;
		x[i].imag = 0;
	}
}
float testmodulemic(int dimx, int dimy) {

	int N1 = dimx, N2 = dimy;

	MKL_LONG N[2];
	N[0] = N1;
	N[1] = N2;

	MKL_Complex8 *x = (MKL_Complex8*) mkl_malloc(
			N[0] * N[1] * sizeof(MKL_Complex8), ALIGN_SIZE);
	MKL_Complex8 *out = (MKL_Complex8*) mkl_malloc(
			N[0] * N[1] * sizeof(MKL_Complex8), ALIGN_SIZE);

	init(x, N[0], N[1]);

	double timeres;

#pragma offload target(mic:1) \
							in(x:length(N[0]*N[1])) \
							inout(out:length(N[0]*N[1]))\
							inout(timeres)
	{
		MKL_LONG status = 0;

		DFTI_DESCRIPTOR_HANDLE handle = 0;
		MKL_LONG ret;
		status = DftiCreateDescriptor(&handle, DFTI_SINGLE, DFTI_COMPLEX, 2, N);

		DftiSetValue(handle, DFTI_PLACEMENT, DFTI_NOT_INPLACE);
		status = DftiCommitDescriptor(handle);
		struct timeval t1, t2;
		double timeuse;

		gettimeofday(&t1, NULL);
		for (int i = 0; i < REP_TIMES; i++)
			 status = DftiComputeBackward(handle,x,out);
			//status = DftiComputeForward(handle, x, out);

		/*
		 for(int i=0;i<REP_TIMES;i++)
		 status = DftiComputeBackward(handle,x,out);
		 */
		gettimeofday(&t2, NULL);
		timeres = 1000 * (t2.tv_sec - t1.tv_sec)+ (t2.tv_usec - t1.tv_usec) / 1000;
		//ms
		DftiFreeDescriptor(&handle);
	}
	return timeres;

}

int main() {

	double timeres[10];

	int pownum = 3;
	//128=2^7   8192=2^13
	for (pownum = 7; pownum <= 13; pownum++) {
		double avertime = 0;
		for (int i = 0; i < 10; i++) {
			timeres[i] = testmodulemic(pow(2, pownum), pow(2, pownum));
			printf("ITER %f ", timeres[i]);
			avertime += timeres[i];
		}
		printf("\n AVER %f \n", avertime / 10);
	}

}
