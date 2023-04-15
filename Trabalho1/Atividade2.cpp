
#include <stdio.h>
#include <iostream>
#include "mpi.h"
#include <math.h>
#include <string>

int main(int argc, char *argv[])
{
    int msg_dest, msg_src;
    int rank, size;
    int input;
    // double start, end;
    double d;

    // Inicializacao
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Obtem dd = log(size)

    // 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
    // https://github.com/raphaelprados/UFGDTrabalho1TopAvanComp3-2022-2

    if(rank == 0) {
        std::cout << "Digite um numero: ";
        std::cin >> input;
    }

    // Inicializa d
    d = log2(size);

    int mask = (int) pow(2.0, d) - 1;
    for(int i = d - 1; i >= 0; i--) {
        mask ^= (int) pow(2.0, i);
        if(!(rank & mask)) {
            if((rank & (int) pow(2.0, i)) == 0) {
                msg_dest = rank ^ (int) pow(2.0, i);
                MPI_Send(&input, 1, MPI_INT, msg_dest, 0, MPI_COMM_WORLD);
                // std::cout << "Process n" << rank << " sent " << input << " to n" << msg_dest << std::endl;
            } else {
                msg_src = rank ^ (int) pow(2.0, i);
                MPI_Recv(&input, 1, MPI_INT, msg_src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                //std::cout << "Process n" << rank << " received " << input << " from n" << msg_src << std::endl;
            }
        }
    }

    std::string str = std::to_string(d) + "ms";
    std::cout << str << std::endl;

    // Finalizacao
    MPI_Finalize();

    return 0;
}