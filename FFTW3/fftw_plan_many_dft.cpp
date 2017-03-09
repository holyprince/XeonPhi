#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fftw3.h>

//4096*23
void init(fftw_complex *x, int Nx, int Ny) {

	for (int i = 0; i < Nx; i++)
		for (int j = 0; j < Ny; j++) {
			x[i * Ny + j][0] = i % 1000;
			x[i * Ny + j][1] = 0;
		}

}

void printmatrix(fftw_complex *x, int Nx, int Ny) {
	for (int i = 0; i < 5; i++) {
		printf("\n");
		for (int j = 0; j < 5; j++) {
			printf("%f+i*%f ", x[i * Ny + j][0], x[i * Ny + j][1]);
		}
	}
	printf("\n");

}

int main() {

	int N, Ny = 30;
	N = 4096;
	fftw_complex *in, *out;
	fftw_plan plan;
	in = (fftw_complex *) fftw_malloc(N * Ny * sizeof(fftw_complex));
	out = (fftw_complex *) fftw_malloc(N * Ny * sizeof(fftw_complex));

	int howmany = Ny;
	// ... if data is column-major, set istride=howmany, idist=1
	//    if data is row-major, set istride=1, idist=N
	//init and the order is important

	plan = fftw_plan_many_dft(1, &N, howmany, in, NULL, howmany, 1, out, NULL, howmany, 1, FFTW_FORWARD, FFTW_MEASURE);
	init(in, 4096, Ny);
	printmatrix(in, 4096, Ny);
	fftw_execute(plan);
	printmatrix(out, 4096, Ny);
	return 0;
}
