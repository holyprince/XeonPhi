
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fftw3.h>

void init(fftw_complex *x, int N);
void print_complex_vector(fftw_complex *x, int N);

int main() {

	int N;
	N = 4096;

	fftw_complex *in, *out;
	fftw_plan plan;
	in = (fftw_complex *) fftw_malloc(N * sizeof(fftw_complex));
	out = (fftw_complex *) fftw_malloc(N * sizeof(fftw_complex));

	// fft excution
	plan = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	init(in, N);
	printf(" in : \n");
	print_complex_vector(in, N);

	fftw_execute(plan);
	fftw_destroy_plan(plan);

	printf(" out : \n");
	print_complex_vector(out, N);

	return 0;
}

void init(fftw_complex *x, int N) {
	int i;

	for (i = 0; i < N; i++) {
		x[i][0] = i % 1000;
		x[i][1] = 0.0;
	}

}

void print_complex_vector(fftw_complex *x, int N) {
	int i;

	for (i = 0; i < 10; i++) {
		if (x[i][1] >= 0.0)
			printf(" [%d] = %f +%fi \n", i, x[i][0], x[i][1]);
		else
			printf(" [%d] = %f %fi \n", i, x[i][0], x[i][1]);
	}
	printf("\n");
}
