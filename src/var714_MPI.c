#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#define Max(a,b) ((a)>(b)?(a):(b))
#define N (2*2*2*2*2*2+2)

float maxeps = 0.1e-7;
int itmax = 100;
int i, j, k;

void relax();
void init();
void verify();

int main(int an, char **as)
{
    int rank, size;
    int it;
    float eps;
    double t0, t1;

    MPI_Init(&an, &as);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rows_per_proc = N / size;
    int remainder = N % size;
    int start_row = rank * rows_per_proc + (rank < remainder ? rank : remainder);
    int nrows = rows_per_proc + (rank < remainder ? 1 : 0);

    float *data = (float *)malloc((nrows + 6) * N * sizeof(float));
    float **A = (float **)malloc((nrows + 6) * sizeof(float *));
    for (i = 0; i < nrows + 6; i++)
        A[i] = &data[i * N];

    t0 = MPI_Wtime();

    init(A, nrows, start_row);

    for (it = 1; it <= itmax; it++)
    {
        eps = 0.;
        relax(A, nrows, rank, size, start_row, &eps);
        
        if (rank == 0)
            printf("it=%4i   eps=%f\n", it, eps);
            
        if (eps < maxeps) break;
    }

    verify(A, nrows, start_row, rank);

    t1 = MPI_Wtime();
    
    if (rank == 0)
        printf("MPI processes: %d  Time: %f seconds\n", size, t1 - t0);

    free(A);
    free(data);
    MPI_Finalize();
    return 0;
}

void init(float **A, int nrows, int start_row)
{
    for (i = 0; i < nrows; i++)
    {
        int gi = start_row + i;
        for (j = 0; j <= N - 1; j++)
        {
            if (gi == 0 || gi == N - 1 || j == 0 || j == N - 1)
                A[i + 3][j] = 0.;
            else
                A[i + 3][j] = (1. + gi + j);
        }
    }
}

void relax(float **A, int nrows, int rank, int size, int start_row, float *eps)
{
    int prev = (rank == 0) ? MPI_PROC_NULL : rank - 1;
    int next = (rank == size - 1) ? MPI_PROC_NULL : rank + 1;
    float local_eps = 0.0;
    MPI_Status status;

    MPI_Sendrecv(&A[nrows][0], 3 * N, MPI_FLOAT, next, 0,
                 &A[0][0], 3 * N, MPI_FLOAT, prev, 0,
                 MPI_COMM_WORLD, &status);

    MPI_Sendrecv(&A[3][0], 3 * N, MPI_FLOAT, prev, 1,
                 &A[nrows + 3][0], 3 * N, MPI_FLOAT, next, 1,
                 MPI_COMM_WORLD, &status);

    for (j = 1; j <= N - 2; j++)
    {
        for (i = 0; i < nrows; i++)
        {
            int gi = start_row + i;
            if (gi >= 3 && gi <= N - 4)
            {
                A[i + 3][j] = (A[i + 2][j] + A[i + 4][j] + 
                               A[i + 1][j] + A[i + 5][j] + 
                               A[i][j] + A[i + 6][j]) / 6.;
            }
        }
    }

    for (i = 0; i < nrows; i++)
    {
        int gi = start_row + i;
        if (gi >= 1 && gi <= N - 2)
        {
            for (j = 3; j <= N - 4; j++)
            {
                float e = A[i + 3][j];
                A[i + 3][j] = (A[i + 3][j - 1] + A[i + 3][j + 1] + 
                               A[i + 3][j - 2] + A[i + 3][j + 2] + 
                               A[i + 3][j - 3] + A[i + 3][j + 3]) / 6.;
                local_eps = Max(local_eps, fabs(e - A[i + 3][j]));
            }
        }
    }

    MPI_Allreduce(&local_eps, eps, 1, MPI_FLOAT, MPI_MAX, MPI_COMM_WORLD);
}

void verify(float **A, int nrows, int start_row, int rank)
{
    float s = 0.f;
    float global_s = 0.f;

    for (i = 0; i < nrows; i++)
    {
        int gi = start_row + i;
        for (j = 0; j <= N - 1; j++)
        {
            s += A[i + 3][j] * (gi + 1) * (j + 1) / (N * N);
        }
    }

    MPI_Reduce(&s, &global_s, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
        printf("  S = %f\n", global_s);
}
