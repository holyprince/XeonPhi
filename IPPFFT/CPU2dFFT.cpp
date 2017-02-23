#include "ipp.h"
#include "ippi.h"
#include <stdio.h>


int main(int argc, char* argv[])
{

   IppStatus           status;
   IppiFFTSpec_C_32fc  *pSpec = NULL;                              /* Pointer to FFT spec structure */

   Ipp32fc             srcFwd[8 * 8] = {0}, dstFwd[8 * 8] = {0};   /* Source/destination images */
   Ipp32fc             m3 = {-3, 0}, one = {1, 0};

   Ipp8u               *pMemInit = NULL, *pBuffer = NULL;          /* Pointer to the work buffers */
   int                 sizeSpec = 0, sizeInit = 0, sizeBuf = 0;    /* Size of FFT spec structure, init and work buffers */

   srcFwd[0] = m3;
   srcFwd[9] = one;

   check_sts( status = ippiFFTGetSize_C_32fc(3, 3, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate, &sizeSpec, &sizeInit, &sizeBuf) )

   /* memory allocation */
   pSpec    = (IppiFFTSpec_C_32fc*) ippMalloc(sizeSpec);
   pBuffer  = (Ipp8u*) ippMalloc(sizeBuf);
   pMemInit = (Ipp8u*) ippMalloc(sizeInit);

   check_sts( status = ippiFFTInit_C_32fc(3, 3, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate, pSpec, pMemInit) )

   /* forward FFT transform */
   check_sts( status = ippiFFTFwd_CToC_32fc_C1R(srcFwd, 8*sizeof(Ipp32fc), dstFwd, 8*sizeof(Ipp32fc), pSpec, pBuffer) )

   ippFree(pMemInit);
   ippFree(pSpec);
   ippFree(pBuffer);
   printf("Exit status %d (%s)\n", (int)status, ippGetStatusString(status));
   return (int)status;

}
