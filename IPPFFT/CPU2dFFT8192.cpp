#include "ipp.h"
#include "ippi.h"
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>

int main(int argc, char* argv[]) {

	IppStatus status;
	IppiFFTSpec_C_32fc *pSpec = NULL; /* Pointer to FFT spec structure */

//	Ipp32fc srcFwd[8 * 8] = { 0 }, dstFwd[8 * 8] = { 0 }; /* Source/destination images */
//	Ipp32fc m3 = { -3, 0 }, one = { 1, 0 };

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

	Ipp8u *pMemInit = NULL, *pBuffer = NULL; /* Pointer to the work buffers */
	int sizeSpec = 0, sizeInit = 0, sizeBuf = 0; /* Size of FFT spec structure, init and work buffers */

	ippiFFTGetSize_C_32fc(13, 13, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
			&sizeSpec, &sizeInit, &sizeBuf);

	/* memory allocation */
	pSpec = (IppiFFTSpec_C_32fc*) ippMalloc(sizeSpec);
	pBuffer = (Ipp8u*) ippMalloc(sizeBuf);
	pMemInit = (Ipp8u*) ippMalloc(sizeInit);

	ippiFFTInit_C_32fc(13, 13, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate, pSpec,
			pMemInit);

	/* forward FFT transform */

	ippiFFTFwd_CToC_32fc_C1R(in, N[0] * sizeof(Ipp32fc), out,
			N[0] * sizeof(Ipp32fc), pSpec, pBuffer);

	for (int i = 0; i < 10; i++)
		printf("%f %f\n", out[i].re, out[i].im);
	printf("\n");

	ippFree(pMemInit);
	ippFree(pSpec);
	ippFree(pBuffer);
	return 0;

}
