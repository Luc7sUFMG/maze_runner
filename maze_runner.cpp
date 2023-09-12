#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <chrono>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mutex> // Para usar mutex
#ifdef _WIN32
#include <windows.h>
#endif

// Matriz de char representando o labirinto
std::vector<std::vector<char>> maze;

// Número de linhas e colunas do labirinto
int num_rows;
int num_cols;

// Representação de uma posição
struct pos_t {
    int i;
    int j;
};

// Estrutura de dados contendo as próximas
// posições a serem exploradas no labirinto
std::stack<pos_t> valid_positions;

// Variável para indicar se a saída foi encontrada
bool exit_found = false;
std::mutex exit_mutex; // Mutex para proteger o acesso à variável exit_found

// Função que lê o labirinto de um arquivo texto, carrega em 
// memória e retorna a posição inicial
pos_t load_maze(const char* file_name) {
    pos_t initial_pos;
    std::ifstream file(file_name);

    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo do labirinto." << std::endl;
        exit(1);
    }

    file >> num_rows >> num_cols;
    maze.resize(num_rows, std::vector<char>(num_cols));

    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            file >> maze[i][j];
            if (maze[i][j] == 'e') {
                initial_pos = {i, j};
            }
        }
    }

    file.close();
    return initial_pos;
}

void maze_print() {
    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            if (maze[i][j] == '.') {
                printf(".");
            } else {
                printf("%c", maze[i][j]);
            }
        }
        printf("\n");
    }
    usleep(40000);
}

bool walk(pos_t start_pos) {
    valid_positions.push(start_pos);

    while (!valid_positions.empty() && !exit_found) { // Continue apenas se a saída não foi encontrada
        pos_t pos = valid_positions.top();
        valid_positions.pop();
        
        if (maze[pos.i][pos.j] == '.') {
            continue;
        }
        
        maze[pos.i][pos.j] = '.';

        system("clear");
        maze_print();

        std::vector<std::thread> threads; // Armazena as threads para caminhos alternativos

        if (pos.i > 0 && (maze[pos.i - 1][pos.j] == 'x' || maze[pos.i - 1][pos.j] == 's')) {
            if (maze[pos.i - 1][pos.j] == 's') {
                // Saída encontrada, defina a variável exit_found dentro do mutex
                std::lock_guard<std::mutex> lock(exit_mutex);
                exit_found = true;
                return true; // Saia da função
            }
            threads.emplace_back(walk, pos_t{pos.i - 1, pos.j}); // Cria uma nova thread para explorar este caminho
        }

        if (pos.i < num_rows - 1 && (maze[pos.i + 1][pos.j] == 'x' || maze[pos.i + 1][pos.j] == 's')) {
            if (maze[pos.i + 1][pos.j] == 's') {
                // Saída encontrada, defina a variável exit_found dentro do mutex
                std::lock_guard<std::mutex> lock(exit_mutex);
                exit_found = true;
                return true; // Saia da função
            }
            threads.emplace_back(walk, pos_t{pos.i + 1, pos.j}); // Cria uma nova thread para explorar este caminho
        }

        if (pos.j > 0 && (maze[pos.i][pos.j - 1] == 'x' || maze[pos.i][pos.j - 1] == 's')) {
            if (maze[pos.i][pos.j - 1] == 's') {
                // Saída encontrada, defina a variável exit_found dentro do mutex
                std::lock_guard<std::mutex> lock(exit_mutex);
                exit_found = true;
                return true; // Saia da função
            }
            threads.emplace_back(walk, pos_t{pos.i, pos.j - 1}); // Cria uma nova thread para explorar este caminho
        }

        if (pos.j < num_cols - 1 && (maze[pos.i][pos.j + 1] == 'x' || maze[pos.i][pos.j + 1] == 's')) {
            if (maze[pos.i][pos.j + 1] == 's') {
                // Saída encontrada, defina a variável exit_found dentro do mutex
                std::lock_guard<std::mutex> lock(exit_mutex);
                exit_found = true;
                return true; // Saia da função
            }
            threads.emplace_back(walk, pos_t{pos.i, pos.j + 1}); // Cria uma nova thread para explorar este caminho
        }

        // Aguarde até que todas as threads terminem antes de continuar
        for (std::thread& t : threads) {
            t.join();
        }
    }

    if (exit_found) {
        return true;
    } else {
        std::cout << "Não foi possível encontrar a saída." << std::endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <nome_do_arquivo_labirinto>" << std::endl;
        return 1;
    }

    pos_t initial_pos = load_maze(argv[1]);
    
    // Iniciar o percurso apenas se a posição inicial for marcada como "e"
    if (maze[initial_pos.i][initial_pos.j] == 'e') {
        bool exit_found = walk(initial_pos);

        if (exit_found) {
            std::cout << "Saída encontrada!" << std::endl;
        }
    } else {
        std::cout << "Posição inicial inválida. Deve ser marcada com 'e'." << std::endl;
    }

    return 0;
}
