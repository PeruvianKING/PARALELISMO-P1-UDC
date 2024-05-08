#include <stdio.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int numprocs, rank, namelen;
    int i, done = 0, n;
    double PI25DT = 3.141592653589793238462643;
    double pi, h, sum, x, sumTotal = 0.0, recibido, salto;
    MPI_Status status;

    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    while (!done)
    {
        sum = 0;
        sumTotal = 0;

        // COMUNICACION
        if (rank == 0)
        {
            printf("Enter the number of intervals: (0 quits) \n");
            scanf("%d", &n);

            for (int i = 1; i < numprocs; i++)
            {
                MPI_Send(&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
        }
        else
            MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        if (n == 0)
            break;

        // CALCULOS
        h = 1.0 / (double)n;

        for (int i = rank; i < n; i += numprocs)
        {
            x = h * ((double)i - 0.5);
            sum += 4.0 / (1.0 + x * x);
        }

        // COMUNICACION
        if (rank == 0)
        {
            sumTotal += sum;
            for (int i = 0; i < numprocs - 1; i++)
            {
                MPI_Recv(&recibido, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
                sumTotal += recibido;
            }
            pi = h * sumTotal;
            printf("pi is approximately %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
        }
        else
            MPI_Send(&sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }                   
    MPI_Finalize();
}
