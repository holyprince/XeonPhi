#include "ipp.h"
#include "ippi.h"
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>


int main(int argc, char* argv[]) {

	IppStatus status;
	int N[2];
	N[0] = 512;
	N[1] = 512;
	Ipp32fc *in, *out;
	in = (Ipp32fc *) malloc(N[0] * N[1] * sizeof(Ipp32fc));
	out = (Ipp32fc *) malloc(N[0] * N[1] * sizeof(Ipp32fc));	
/*for (int x = 0; x < N[0] * N[1]; x++) {
		in[x].re = x % 1000;
		in[x].im = 0;
	}
*/
#pragma offload target(mic:0) inout(out:length(N[0]*N[1])) \
                                      in(in:length(N[0]*N[1]))

	{
		struct timeval tv1, tv2;
		struct timezone tz;
		ippInit();
		gettimeofday(&tv1, &tz);
		for (int x = 0; x < N[0] * N[1]; x++) {
			in[x].re = x % 1000;
			in[x].im = 0;
		}
		IppiFFTSpec_C_32fc *pSpec = NULL; /* Pointer to FFT spec structure */
		Ipp8u *pMemInit = NULL, *pBuffer = NULL; /* Pointer to the work buffers */
		int sizeSpec = 0, sizeInit = 0, sizeBuf = 0; /* Size of FFT spec structure, init and work buffers */
		pSpec = (IppiFFTSpec_C_32fc*) ippMalloc(sizeSpec);
		pBuffer = (Ipp8u*) ippMalloc(sizeBuf);
		pMemInit = (Ipp8u*) ippMalloc(sizeInit);

		ippiFFTInit_C_32fc(N[0], N[1], IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
				pSpec, pMemInit);
		ippiFFTGetSize_C_32fc(N[0], N[1], IPP_FFT_DIV_INV_BY_N,
				ippAlgHintAccurate, &sizeSpec, &sizeInit, &sizeBuf);
		for (int x = 0; x < 100; x++) {
			ippiFFTFwd_CToC_32fc_C1R(in, 8 * sizeof(Ipp32fc), out,
					8 * sizeof(Ipp32fc), pSpec, pBuffer);

		}
		gettimeofday(&tv2, &tz);

		double timeuse = tv2.tv_sec - tv1.tv_sec
				+ (tv2.tv_usec - tv1.tv_usec) / 1000000.0;
		printf(" exact time    FFT  tv_sec:  %f s \n", timeuse);
		printf(" pureFFT   FFT  tv_sec:  %f s \n", tv2.tv_sec - tv1.tv_sec);
		ippFree(pMemInit);
		ippFree(pSpec);
		ippFree(pBuffer);
	}
	for (int i = 0; i < 10; i++)
		printf("%f %f\n", out[i].re, out[i].im);
	printf("\n");

}
