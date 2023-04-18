
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include "mpi.h"

/* Declarações de Funções */
int menu();

struct strct_voo {
    bool ativo;
    int codigo;
    int origem;
    int destino;
    int partida;
    int chegada;

    std::string toString() {
        std::string temp = "\n| Codigo: " + std::to_string(codigo) + " |\nOrigem: " + std::to_string(origem) 
                            + " | " + "\nDestino: " + std::to_string(destino) + " |\nPartida: " 
                            + std::to_string(partida) + " | " +"\nChegada: " + std::to_string(chegada) + " | ";
        return temp;
    }
};

class Voo {
private:
    bool ativo;
public:
    int codigo;
    int origem;
    int destino;
    int partida;
    int chegada;

    // Construtores
    Voo() {
        this->codigo = -1;
        this->origem = -1;
        this->destino = -1;
        this->partida = -1;
        this->chegada = -1;
        this->ativo = false;
    }

    Voo(int codigo, int origem, int destino, int partida, int chegada) {
        this->codigo = codigo;
        this->origem = origem;
        this->destino = destino;
        this->partida = partida;
        this->chegada = chegada;
        this->ativo = true;
    }

    // Getters
    bool getAtivo() { return ativo; }

    // Funcoes quaisquer
    void pousoDecolagem() {
        this->ativo = false;
    }

    // Funcoes de overloading
    friend std::ostream& operator<<(std::ostream& os, Voo& voo);
    bool operator>=(Voo const& voo) {
        // Se dois voos chegam ao mesmo tempo, tem privilegio os com maior tempo de voo
        if(this->chegada == voo.chegada)
            return ((this->chegada - this->partida) > (voo.chegada - voo.partida) ? true : false);
        // Se dois voos partem ao mesmo tempo, tem privilegio o que foi cadastrado primeiro
        else if(this->partida == voo.partida)
            return true;
        // se dois voos partem e chegam ao mesmo tempo
        else
            return {(this->chegada < voo.chegada) || (this->chegada == voo.chegada ? 
                    (this->chegada - this->partida) > (voo.chegada - voo.partida) : true)};
    }
    void operator=(strct_voo const& struct_voo) {
        this->ativo = struct_voo.ativo;
        this->codigo = struct_voo.codigo;
        this->origem = struct_voo.origem;
        this->destino = struct_voo.destino;
        this->partida = struct_voo.partida;
        this->chegada = struct_voo.chegada;
    }
    void operator+=(int r) {
        if(r == 1) {
            this->partida++;
            this->chegada++;    
        } else if(r == 0) {
            this->chegada++;
        }
    }

    // Conversao voo -> struct 
    strct_voo toStructVoo() {
        struct strct_voo voo_struct;

        voo_struct.ativo = this->ativo;
        voo_struct.chegada = this->chegada;
        voo_struct.codigo = this->codigo;
        voo_struct.destino = this->destino;
        voo_struct.origem = this->origem;
        voo_struct.partida = this->partida;

        return voo_struct;
    }
    std::string toString(bool decolagem) {
        std::string temp;
        temp = std::to_string(this->codigo) + " " + std::to_string(decolagem ? this->destino : this->origem) + " " 
                + (decolagem ? std::to_string(this->partida) : std::to_string(this->chegada)) + " "
                + std::to_string(this->chegada - this->partida);
        return temp;
    }
};
std::ostream& operator<<(std::ostream& os, Voo& voo) {
    os << "\n| Codigo: " << voo.codigo << " | " <<
          "\nOrigem: " << voo.origem << " | " <<
          "\nDestino: " << voo.destino << " | " <<
          "\nPartida: " << voo.partida << " | " <<
          "\nChegada: " << voo.chegada << " | ";
    return os;
}


class Aeroporto {
private:
    int codigo;
    int pousos;
    int decolagens;
    int timer;
    int pendentes;
public:
    // Variaveis publicas
    std::vector<Voo> saidas;
    std::vector<Voo> chegadas;

    // Construtores
    Aeroporto() {
        this->codigo = -1;
        this->pousos = 0;
        this->decolagens = 0;
        this->timer = 0;
        this->pendentes = 0;
    }

    Aeroporto(int codigo) {
        this->codigo = codigo;
        this->pousos = 0;
        this->decolagens = 0;
        this->timer = 0;
        this->pendentes = 0;
    }

    // Outras funcoes
    void clockTimer() { timer++; }
    strct_voo criarVoo() {
        int input[3];
        Voo temp_voo = Voo();

        // Input dos parametros do voo
        std::cout << "Digite o codigo do aeroporto destino: ";
        std::cin >> input[0];
        std::cout << "Digite o horario de partida: ";
        std::cin >> input[1];
        std::cout << "Digite o tempo de voo: ";
        std::cin >> input[2];
        
        // Cria o struct do voo
        struct strct_voo struct_voo;
        struct_voo.ativo = true;
        struct_voo.codigo = 100 * (this->codigo + 1) + saidas.size();
        struct_voo.destino = input[0];
        struct_voo.partida = input[1];
        struct_voo.chegada = struct_voo.partida + input[2];
        struct_voo.origem = this->codigo;
        
        // Limpa a tela
        system("clear");

        // Verifica se o horario do novo voo nao tem conflito com um ja programado
        if(conflitos(temp_voo))
            temp_voo+=1;

        // Salva o estado de lançamento de voos
        temp_voo = struct_voo;
        saidas.push_back(temp_voo);

        // Atualiza o numero de voos que ainda precisam ser comunicados
        pendentes++;
    
        return struct_voo;
    }

    bool conflitos(Voo voo) {
        // Verifica se há outra decolagem para o horário da atual
        for(long unsigned i = 0; i < saidas.size(); i++)
            if(saidas[i].getAtivo() && saidas[i].partida == voo.partida)
                return true;
        // Verifica se há um pouso no mesmo horario da decolagem atual
        for(long unsigned i = 0; i < chegadas.size(); i++)
            if(chegadas[i].getAtivo() && chegadas[i].chegada == voo.partida)
                return true;
        return false;
    }

    // Getters
    int getCodigo() { return codigo; }
    int getPousos() { return pousos; }
    int getDecolagens() { return decolagens; }
    int getTimer() { return timer; }
    int getPendentes() { return pendentes; }

    // Outras funcoes
    void clearPendentes() { this->pendentes = 0; }
    std::string toString() {
        std::string dados;

        dados = "Codigo: " + std::to_string(this->codigo) + "\n" +
            "Pousos: " + std::to_string(this->pousos) + " Decolagens: " + std::to_string(this->decolagens) + "\n" +
            "Pousos | Origem | Chegada | Tempo Voo\n";
        for(long unsigned i = 0; i < chegadas.size(); i++)
            dados += chegadas[i].toString(false) + "\n";
        dados += "Decolagens | Destino | Partida | Tempo de Voo\n"; 
        for(long unsigned i = 0; i < saidas.size(); i++)
            dados += saidas[i].toString(true) + "\n";

        return dados;
    }

    // Funcao principal
    void executar(int size) {
        int input = -1;

        while(input != 0) {
            std::cout << "Timer: " << timer << std::endl;
            std::cout << toString() << std::endl;
            input = menu();
            system("clear");            
            switch(input) {
                case 1:
                    // Gerando os dados do Voo
                    criarVoo();
                    break;
                case 2:    
                    /* Trecho de Comunicação */
                    std::cout << "Aguardando outros aeroportos..." << std::endl;
                    MPI_Barrier(MPI_COMM_WORLD);
                    /* Trecho de ordenacao e execucao de pousos/decolagens */
                    char result = escalonarVoos();
                    if(result == 'p') {
                        unsigned long c = 0;                        
                        for(; c < chegadas.size() && !chegadas[c].getAtivo(); c++) {}                        
                        if(c < chegadas.size()) {
                            chegadas[c].pousoDecolagem();
                            pousos++;
                        }
                    } else if(result == 'd') {
                        unsigned long s = 0;
                        for(; s < saidas.size() && !saidas[s].getAtivo(); s++) {}
                        if(s < saidas.size()) {
                            saidas[s].pousoDecolagem();
                            decolagens++;
                        }
                    }
                    
                    //Recomunica os voos, possivelmente modificados
                    comunicacao(size);
                    timer++;
                    system("clear");
                    break;
            }
        }
    }

    void comunicacao(int size) {
        bool destinos[size];
        strct_voo struct_voo;
        Voo voo;
        
        memset(destinos, 0, size); // Seta todas as posicoes como false

        for(int i = 0; i < size; i++) {
            // Define com quem o aeroporto precisa se comunicar
            long unsigned c = saidas.size() - pendentes;
            
            if(this->codigo == i) {
                for(; c < saidas.size(); c++) {
                    for(int j = 0; j < size; j++) {
                        if(saidas[c].destino == j) {
                            destinos[j] = true;
                        }
                    }
                }
            }
            // Comunica os outros aeroportos sobre seus voos
            MPI_Bcast(&destinos, size, MPI_BYTE, i, MPI_COMM_WORLD);

            c = saidas.size() - pendentes;
            // Faz os send e receives para comunicar a cada aeroporto um novo pouso
            for(int j = 0; j < size; j++) {
                if(this->codigo == i && destinos[j]) {
                    struct_voo = saidas[c].toStructVoo();
                    MPI_Send(&struct_voo, sizeof(struct_voo), MPI_BYTE, j, i, MPI_COMM_WORLD);
                    c++;
                    // Evita que um voo seja identificado mais de uma vez
                    destinos[j] = 0;
                } else if(this->codigo == j && destinos[j]) {
                    MPI_Recv(&struct_voo, sizeof(struct_voo), MPI_BYTE, i, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    voo = struct_voo;
                    chegadas.push_back(voo);
                    // Evita que um voo seja identificado mais de uma vez
                    destinos[j] = 0;
                }
            }
        }
        // Valida o numero de envios pendentes para o aeroporto enviante
        this->pendentes = 0;
        
    }

    void sortSaidas(int s, int r) {
        Voo temp;
        if(s >= r)
            return;
        for(int i = s; i < r; i++) {
            for(int j = s; j < r - 1; j++) {
                if(saidas[j] >= saidas[j+1]) {
                    temp = saidas[j];
                    saidas[j] = saidas[j+1];
                    saidas[j+1] = temp;
                }
            }
        }
    }

    void sortChegadas(int c, int r) {
        Voo temp;
        if(c >= r)
            return;
        for(int i = c; i < r; i++) {
            for(int j = c; j < r - 1; j++) {
                if(chegadas[j] >= chegadas[j+1]) {
                    temp = chegadas[j];
                    chegadas[j] = chegadas[j+1];
                    chegadas[j+1] = temp;
                }
            }
        }
    }

    char escalonarVoos() {
        Voo temp;
        // Obtem os indices dos proximos voos 
        unsigned long s = 0;
        unsigned long c = 0;
        while(s < saidas.size() && !saidas[s].getAtivo()) { s++; } // 
        while(c != chegadas.size() && !chegadas[c].getAtivo()) { c++; }
        // Sort das saidas
        sortSaidas(s, saidas.size());
        // Sort das chegadas
        sortChegadas(c, chegadas.size());

        if(s < saidas.size()) {
            if(c < chegadas.size()) {
                // Não ha pousos nem decolagens
                if(saidas[s].partida != timer && chegadas[c].chegada != timer)
                    return 'n';
                // Ha um pouso ou uma decolagem, de acordo com a prioridade do escalonamento
                else {
                    if(saidas[s].partida == chegadas[c].chegada)
                        saidas[s]+=1;
                    return {((saidas[s].partida >= chegadas[c].chegada) ? 
                                'p' : 'd')};
                }                    
            } else {
                return (saidas[s].partida == timer ? 'd' : 'n');
            }
        } else if(c < chegadas.size()) {
            return (chegadas[c].chegada == timer ? 'p' : 'n');
        } else {
            return 'n';
        }
        
        return 'f';
    }

    // Funcoes Friend
    friend std::ostream& operator<<(std::ostream& os, Aeroporto& aeropt); 
};
std::ostream& operator<<(std::ostream& os, Aeroporto& aeropt) {
    os << aeropt.toString();
    return os;
}


int menu() {
    int input;

    std::cout << "-----------------------" <<
                 "\n|1. Criar Voo       |" << 
                 "\n|2. Executar        |" <<
                 "\n|0. Finalizar       |" <<
                 "\n-----------------------" << std::endl;
    std::cout << "> ";
    std::cin >> input;

    return input;    
}

int main(int argc, char *argv[]){
    
    int rank, size;
    Aeroporto aeropt;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    aeropt = Aeroporto(rank);   

    aeropt.executar(size);

    MPI_Finalize();

    return 0;
}



