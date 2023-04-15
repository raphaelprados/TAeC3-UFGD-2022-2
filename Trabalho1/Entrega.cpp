
/*
    Disciplina: Tópicos Avançados em Computação III
    Curso: Engenharia da Computação - UFGD - FACET
    Docente: Rodrigo Porfírio da Silva Sacchi
    Discentes: Giovane Lagustera Silvestrim
               Raphael Alexsander Prado dos Santos

    Trabalho: implementação de dispersão linear e em hipercubo através da
              biblioteca OpenMPI, em C++, utilizando o sistema operacional
              Linux. 

    Data: 07/03/2023
    Local: Dourados, MS
*/

#include <stdio.h>
#include <iostream>
#include "mpi.h"
#include <math.h>
#include <iomanip>
#include <unistd.h>
#include <string>

int menu();
double LinearDispersion(int number, int rank, int size);
double HypercubeDispersion(int number, int rank, int size);
void orderlyPrint(int rank, int size, std::string execution_log, double execution_time);

int main(int argc, char *argv[]) {
    
    // Variáveis
    int menu_input;
    int number_input = 25;
    int size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    do {
        if(rank == 0)
            menu_input = menu();
        MPI_Barrier(MPI_COMM_WORLD);
        // Envia o resultado de menu_input para as outras maquinas fazerem a chamada da funcao
        MPI_Bcast(&menu_input, 1, MPI_INT, 0, MPI_COMM_WORLD);
        switch (menu_input) {
            case 1:
                if(rank == 0)
                    std::cout << "Algoritmo Linear" << std::endl;
                LinearDispersion(number_input, rank, size);
                break;
            case 2:
                if(rank == 0)
                    std::cout << "Algoritmo de Hipercubo" << std::endl;
                HypercubeDispersion(number_input, rank, size);
                break;
        }
        MPI_Barrier(MPI_COMM_WORLD);
    } while(menu_input != 0);

    MPI_Finalize();

    return 0;
}

int menu() {

    int input = 0;

    std::cout << "---------------------" << std::endl;
    std::cout << "|Selecione sua opcao| " << std::endl;
    std::cout << "|1. Linear          |" << std::endl;
    std::cout << "|2. Hypercubo       |" << std::endl;
    std::cout << "|0. Sair            |" << std::endl;
    std::cout << "---------------------" << std::endl;
    std::cout << " >";
    std::cin >> input;
    
    while(input < 0 || input > 2) {
        std::cout << "Digite uma opcao valida!!!" << std::endl;
        std::cin >> input;
    }

    return input;
}

double LinearDispersion(int number, int rank, int size) {
    // Declaracao de variaveis
    int input, output;
    double start, end;
    std::string execution_log;
    std::string temp;

    // Obtendo o numero do usuario
    if(rank == 0) {
        std::cout << "Digite um numero: ";
        std::cin >> input;
        std::cout << "Numero: " << input << std::endl;
    }

    // Inicia o contador para cada maquina
    start = MPI_Wtime();

    // Estes dois laços for geram a sequência de numeros equivalente aos ranks das maquinas
    // Por exemplo, para size = 8, ele gera a sequencia:
    // 0 -> 4
    // 0 -> 2 | 4 -> 2
    // 0 -> 1 | 2 -> 3 | 4 -> 5 | 6 -> 7

    for(int i = size / 2; i != 0; i/= 2) 
        for(int j = 0; j < size; j += 2 * i) {
            if(rank == j + i) {
                MPI_Recv(&output, 1, MPI_INT, j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                input = output;
                // Os valores para execution log servem para ordenar a impressão do log 
                end = MPI_Wtime();
                execution_log.append("Process n" + std::to_string(j + i) + " received " + std::to_string(output) + " from n" + std::to_string(j) + "\n");
                // execution_log.append("Process n" + std::to_string(j + i) + " received " + std::to_string(output) + " from n" + j + "\n");
                // std::cout << execution_log << std::endl;
            } else if(rank == j) {
                MPI_Send(&input, 1, MPI_INT, j + i, 0, MPI_COMM_WORLD);
                end = MPI_Wtime();
                execution_log.append("Process n" + std::to_string(j) + " sent " + std::to_string(input) + " to n" + std::to_string(j + i) + " at " + std::to_string((end - start) * 1000) + "ms\n");
                // std::cout << "Process n" << j << " sent " << input << "to n" << j + i << " at " << i << std::endl;
            }
        }

    //Finaliza o contador para cada maquina
    end = MPI_Wtime();
    execution_log.append("Process n" + std::to_string(rank) + " finished on " + std::to_string((end - start)*1000) + "ms\n");
    
    orderlyPrint(rank, size, execution_log, ((end - start) * 1000));

    return 0.0;
}

double HypercubeDispersion(int number, int rank, int size) {
    
    int msg_dest, msg_src;
    int input;
    double start, end;
    double d;
    std::string execution_log;

    if(rank == 0) {
        std::cout << "Digite um numero: ";
        std::cin >> input;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    start = MPI_Wtime();

    // Inicializa d
    d = log2(size);

    int mask = (int) pow(2.0, d) - 1;
    for(int i = d - 1; i >= 0; i--) {
        mask ^= (int) pow(2.0, i);
        if(!(rank & mask)) {
            if((rank & (int) pow(2.0, i)) == 0) {
                msg_dest = rank ^ (int) pow(2.0, i);
                MPI_Send(&input, 1, MPI_INT, msg_dest, 0, MPI_COMM_WORLD);
                end = MPI_Wtime();
                execution_log.append("Process n" + std::to_string(rank) + " sent " + std::to_string(input) + " to n" + std::to_string(msg_dest) + " at " + std::to_string((end-start)*1000) + "ms\n");
                // std::cout << "Process n" << rank << " sent " << input << " to n" << msg_dest << " at " << (end-start)*1000 << "ms" << std::endl;
            } else {
                msg_src = rank ^ (int) pow(2.0, i);
                MPI_Recv(&input, 1, MPI_INT, msg_src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                end = MPI_Wtime();
                execution_log.append("Process n" + std::to_string(rank) + " received " + std::to_string(input) + " from n" + std::to_string(msg_src) + " at " + std::to_string((end-start)*1000) + "ms\n");
                // std::cout << "Process n" << rank << " received " << input << " from n" << msg_src << " at " << (end-start)*1000 << "ms"  << std::endl;
            }
        }
    }

    // Finaliza a contagem de tempo
    end = MPI_Wtime();

    // Imprime com barreira, para facilitar a leitura
    execution_log.append("Process n" + std::to_string(rank) + " finished on " + std::to_string((end - start)*1000) + "ms\n");
    
    orderlyPrint(rank, size, execution_log, ((end - start) * 1000));

    return 0.0;
}

void orderlyPrint(int rank, int size, std::string execution_log, double execution_time) {

    double total_exec_time;
    double partial_exec_time = execution_time;

    // Configuração para output com duas casas decimais
    std::cout << std::fixed;
    std::cout << std::setprecision(3);

    for(int i = 0; i < size; i++) {
        if(rank == i) 
            std::cout << execution_log << std::endl;
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Reduce(&partial_exec_time, &total_exec_time, 1, MPI_DOUBLE, MPI_MAX, 
                    0, MPI_COMM_WORLD);
    if(rank == 0)
        std::cout << "Total execution time: " << total_exec_time << "ms" << std::endl;

}
