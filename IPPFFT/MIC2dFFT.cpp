
#include "ipp.h"
#include "ippi.h"
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>

int main(int argc, char* argv[]) {

	int N[2];
	N[0] = 4096 * 2;
	N[1] = 4096 * 2;
	Ipp32fc *in, *out;
	in = (Ipp32fc *) malloc(N[0] * N[1] * sizeof(Ipp32fc));
	out = (Ipp32fc *) malloc(N[0] * N[1] * sizeof(Ipp32fc));
	for (int x = 0; x < N[0] * N[1]; x++) {
		in[x].re = x % 1000;
		in[x].im = 0;
	}
#pragma offload target(mic:0) inout(out:length(N[0]*N[1])) \
        								in(in:length(N[0]*N[1]))
	{

		struct timeval tv1, tv2;
		struct timezone tz;
		ippInit();
		Ipp8u *pMemInit = NULL, *pBuffer = NULL;
		int sizeSpec = 0, sizeInit = 0, sizeBuf = 0;
		IppiFFTSpec_C_32fc *pSpec = NULL;
		//the order is important for GetSize and Init_C
		ippiFFTGetSize_C_32fc(13, 13, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
				&sizeSpec, &sizeInit, &sizeBuf);

		pSpec = (IppiFFTSpec_C_32fc*) ippMalloc(sizeSpec);
		pBuffer = (Ipp8u*) ippMalloc(sizeBuf);
		pMemInit = (Ipp8u*) ippMalloc(sizeInit);

		ippiFFTInit_C_32fc(13, 13, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
				pSpec, pMemInit);

		gettimeofday(&tv1, &tz);
		for (int i = 0; i < 100; i++) {
			ippiFFTFwd_CToC_32fc_C1R(in, N[0] * sizeof(Ipp32fc), out,
					N[0] * sizeof(Ipp32fc), pSpec, pBuffer);
		}
		gettimeofday(&tv2, &tz);
		double timeuse = tv2.tv_sec - tv1.tv_sec
				+ (tv2.tv_usec - tv1.tv_usec) / 1000000.0;
		printf(" exact time    FFT  tv_sec:  %f s \n", timeuse);
		printf(" pureFFT   FFT  tv_sec:  %f s \n", tv2.tv_sec - tv1.tv_sec);

		for (int i = 0; i < 10; i++)
			printf("%f %f\n", out[i].re, out[i].im);
		printf("\n");

		ippFree(pMemInit);
		ippFree(pSpec);
		ippFree(pBuffer);
	}
	return 0;

}
