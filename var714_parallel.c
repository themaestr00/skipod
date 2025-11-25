#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#define  Max(a,b) ((a)>(b)?(a):(b))

#define  N   (2*2*2*2*2*2+2)
float   maxeps = 0.1e-7;
int itmax = 100;
int i,j,k;

float eps;
float A [N][N];

void relax();
void init();
void verify(); 

int main(int an, char **as)
{
	double t0 = omp_get_wtime();
	int it;
	init();
	int threads_used = 0;

	for(it=1; it<=itmax; it++)
	{
		eps = 0.;
		relax();
		printf( "it=%4i   eps=%f\n", it,eps);
		if (eps < maxeps) break;
	}

	verify();

	double t1 = omp_get_wtime();
	#pragma omp parallel
	{
		#pragma omp single
		threads_used = omp_get_num_threads();
	}
	printf("Threads: %d  Time: %f seconds\n", threads_used, t1 - t0);
	return 0;
}


void init()
{ 
	#pragma omp parallel for collapse(2) schedule(static)
	for(j=0; j<=N-1; j++)
	for(i=0; i<=N-1; i++)
	{
		if(i==0 || i==N-1 || j==0 || j==N-1) A[i][j]= 0.;
		else A[i][j]= ( 1. + i + j ) ;
	}
} 

void relax()
{
    float local_eps = 0.0;

    #pragma omp parallel reduction(max:local_eps)
    {
        #pragma omp for schedule(static) private(i, j)
        for (j=1; j<=N-2; ++j)
        for (i=3; i<=N-4; ++i)
        {
            A[i][j] = (A[i-1][j]+A[i+1][j]+A[i-2][j]+A[i+2][j]+A[i-3][j]+A[i+3][j])/6.;
        }
        #pragma omp for schedule(static) private(i, j)
        for (i=1; i<=N-2; ++i)
        for (j=3; j<=N-4; ++j)
        {
            float e;
            e=A[i][j];
            A[i][j] =(A[i][j-1]+A[i][j+1]+A[i][j-2]+A[i][j+2]+A[i][j-3]+A[i][j+3])/6.;
            local_eps=Max(local_eps,fabs(e-A[i][j]));
        }
    }

    eps = local_eps;
}

void verify()
{ 
    float s = 0.f;
    #pragma omp parallel for collapse(2) reduction(+:s) schedule(static)
    for(j=0; j<=N-1; j++)
    for(i=0; i<=N-1; i++)
    {
        s += A[i][j]*(i+1)*(j+1)/(N*N);
    }
    printf("  S = %f\n", s);
}
