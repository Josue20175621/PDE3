#include <iostream>
#include <mpi.h>

#define MASTER_RANK 0

#define NOT_ENOUGH_PROCESSES_NUM_ERROR 1

int main(int argc, char *argv[])
{
    int communicator_size;
    int rank;
    int process_id;

    // Intializes the MPI execution environment
    MPI_Init(&argc, &argv);

    // Returns the size of the group associated with a communicator
    MPI_Comm_size(MPI_COMM_WORLD, &communicator_size);

    // Determines the rank of the calling process in the communicator
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Si el numero de procesos es menor que 2, ABORT
    if (communicator_size < 2)
    {
        MPI_Abort(MPI_COMM_WORLD, NOT_ENOUGH_PROCESSES_NUM_ERROR);
    }

    /* ---------------------------------- MASTER ----------------------------------- */
    
    if (rank == MASTER_RANK)
    {
        std::cout << "MASTER" << std::endl;
    }

    /* ---------------------------------- WORKER ----------------------------------- */

    if (rank > MASTER_RANK)
    {
        std::cout << "WORKER" << std::endl;
    }
    
    MPI_Finalize();
    return 0;
}