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
float testmodule(int dimx,int dimy) {
	int N1 = dimx, N2 = dimy;

	MKL_LONG N[2];
	N[0] = N1;
	N[1] = N2;

	MKL_Complex8 *x = (MKL_Complex8*) mkl_malloc(N[0] * N[1] * sizeof(MKL_Complex8), ALIGN_SIZE);
	MKL_Complex8 *out = (MKL_Complex8*) mkl_malloc(N[0] * N[1] * sizeof(MKL_Complex8), ALIGN_SIZE);

	init(x, N[0], N[1]);

	{
		MKL_LONG status = 0;

		DFTI_DESCRIPTOR_HANDLE handle = 0;
		MKL_LONG ret;
		status = DftiCreateDescriptor(&handle, DFTI_SINGLE, DFTI_COMPLEX, 2, N);

		DftiSetValue(handle, DFTI_PLACEMENT, DFTI_NOT_INPLACE);
		status = DftiCommitDescriptor(handle);
		struct timeval t1, t2, diff;
		double timeuse;

		gettimeofday(&t1, NULL);
		for (int i = 0; i < REP_TIMES; i++)
			status = DftiComputeForward(handle, x, out);
		gettimeofday(&t2, NULL);
		//double timeinsec = (diff.tv_sec * 1000 + diff.tv_usec / 1000) / 1000.0;
		//timeuse = t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) / 1000000.0;
		return 1000 * (t2.tv_sec-t1.tv_sec)+ (t2.tv_usec-t1.tv_usec)/1000;
#ifdef PRINT
		printf("Use timeinsec of FFT :%f\n", timeinsec);
		printf("Use Time of FFT :%f\n", timeuse);
		printf("FFT tv_sec:  %f s \n", t2.tv_sec - t1.tv_sec);
#endif
		DftiFreeDescriptor(&handle);
		//double perf = (REP_TIMES * 2.5 * N[0] * N[1] * log2(1.0 * N[0] * N[1]))/ timeinsec / 1e9;
		//printf("performance: %f GFLOPS\n", perf);
	}
#ifdef PRINT
	for (int i = 0; i < 10; i++)
		printf("%f %f\n", out[i].real, out[i].imag);
#endif
}

int main() {

	double timeres[10];
	double avertime=0;
	for(int i=0;i<10;i++)
	{
		timeres[i]=testmodule(128,128);
		printf("ITER %f ",timeres[i]);
		avertime+=timeres[i];
	}
	printf("\n AVER %f \n",avertime/10);
}
