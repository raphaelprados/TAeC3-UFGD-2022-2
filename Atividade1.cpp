
#include <stdio.h>
#include <iostream>
#include <mpi.h>

int main(int argc, char *argv[]) {
    
    // Declaracao de variaveis
    int size, rank, input, output;
    double start, end;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Obtendo o numero do usuario

    if(rank == 0) {
        std::cout << "Digite um numero: ";
        std::cin >> input;
        std::cout << "Numer0: " << input << std::endl;
        for(int i = 0; i < size; i++)
            MPI_Send(&input, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
    } else
        MPI_Recv(&input, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Inicia o contador para cada maquina

    start = MPI_Wtime();

    for(int i = size / 2; i != 0; i/= 2) 
        for(int j = 0; j < size; j += 2 * i) {
            if(rank == j + i) {
                MPI_Recv(&output, 1, MPI_INT, j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                input = output;
                std::cout << "Process n" << j + i << " received " << output << " from n" << j << std::endl;
            } else if(rank == j) {
                MPI_Send(&input, 1, MPI_INT, j + i, 0, MPI_COMM_WORLD);
                std::cout << "Process n" << j << " sent " << input << "to n" << j + i << " at " << i << std::endl;
            }
        }

    //Finaliza o contador para cada maquina

    end = MPI_Wtime();

    std::cout << "Process n" << rank << " finished on " << end - start << "s" << std::endl;

    MPI_Finalize();

    return 0;
}
