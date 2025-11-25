#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
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
    clock_t start = clock();
	int it;
	init();

	for(it=1; it<=itmax; it++)
	{
		eps = 0.;
		relax();
		printf( "it=%4i   eps=%f\n", it,eps);
		if (eps < maxeps) break;
	}

	verify();
    printf("Time taken: %f seconds\n", (double)(clock() - start) / CLOCKS_PER_SEC);

	return 0;
}


void init()
{ 
	for(j=0; j<=N-1; j++)
	for(i=0; i<=N-1; i++)
	{
		if(i==0 || i==N-1 || j==0 || j==N-1) A[i][j]= 0.;
		else A[i][j]= ( 1. + i + j ) ;
	}
} 

void relax()
{
	for(j=1; j<=N-2; j++)
	for(i=3; i<=N-4; i++)
	{
		A[i][j] = (A[i-1][j]+A[i+1][j]+A[i-2][j]+A[i+2][j]+A[i-3][j]+A[i+3][j])/6.;
	}

	for(j=3; j<=N-4; j++)
	for(i=1; i<=N-2; i++)
	{
		float e;
		e=A[i][j];
		A[i][j] =(A[i][j-1]+A[i][j+1]+A[i][j-2]+A[i][j+2]+A[i][j-3]+A[i][j+3])/6.;
		eps=Max(eps,fabs(e-A[i][j]));
	}

}

void verify()
{ 
	float s;

	s=0.;
	for(j=0; j<=N-1; j++)
	for(i=0; i<=N-1; i++)
	{
		s=s+A[i][j]*(i+1)*(j+1)/(N*N);
	}
	printf("  S = %f\n",s);

}