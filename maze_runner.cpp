#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <chrono>
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
// Função que imprime o labirinto com um atraso de 0.5 segundos
void print_maze(const pos_t& current_pos) {
    system("cls"); // Limpar a tela (funciona em sistemas Windows)
    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            if (i == current_pos.i && j == current_pos.j) {
                std::cout << 'o'; // Posição corrente
            } else {
                std::cout << maze[i][j];
            }
        }
        std::cout << std::endl;
    }

#ifdef _WIN32
    Sleep(500); // Atraso de 0.5 segundos (Windows)
#else
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Atraso de 0.5 segundos (outros sistemas)
#endif
}

// Função responsável pela navegação.
// Recebe como entrada a posição inicial e retorna um booleano indicando se a saída foi encontrada
bool walk(pos_t pos) {
    valid_positions.push(pos);

    while (!valid_positions.empty()) {
        pos = valid_positions.top();
        valid_positions.pop();

        // Marcar a posição atual como explorada
        maze[pos.i][pos.j] = '.';

        print_maze(pos);

        // Verificar se a saída foi encontrada
        if (maze[pos.i][pos.j] == 's') {
            return true;
        }

        // Verificar as próximas posições válidas e adicioná-las à pilha
        if (pos.i > 0 && maze[pos.i - 1][pos.j] == 'x') {
            valid_positions.push({pos.i - 1, pos.j});
        }
        if (pos.i < num_rows - 1 && maze[pos.i + 1][pos.j] == 'x') {
            valid_positions.push({pos.i + 1, pos.j});
        }
        if (pos.j > 0 && maze[pos.i][pos.j - 1] == 'x') {
            valid_positions.push({pos.i, pos.j - 1});
        }
        if (pos.j < num_cols - 1 && maze[pos.i][pos.j + 1] == 'x') {
            valid_positions.push({pos.i, pos.j + 1});
        }
    }

    return false; // Saída não encontrada
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <nome_do_arquivo_labirinto>" << std::endl;
        return 1;
    }

    pos_t initial_pos = load_maze(argv[1]);
    bool exit_found = walk(initial_pos);

    if (exit_found) {
        std::cout << "Saída encontrada!" << std::endl;
    } else {
        std::cout << "Não foi possível encontrar a saída." << std::endl;
    }

    return 0;
}
