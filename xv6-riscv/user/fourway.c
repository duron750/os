#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Generator simplu de numere pseudo-aleatorii
unsigned int next = 1;
int my_rand(void) {
    next = next * 1103515245 + 12345;
    return (unsigned int)(next / 65536) % 32768;
}

void print_board(int *board, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%d ", board[i * size + j]);
        }
        printf("\n");
    }
}

void simulate(int size) {
    int board[25]; // Suportă până la 5x5
    int r, c, idx, val, steps = 0;
    int center = size / 2;
    
    for(int i = 0; i < size * size; i++) {
        board[i] = (my_rand() % 4) + 1;
    }

    r = center;
    c = center;
    printf("Configuratia initiala a tablei:\n");
    print_board(board, size);
    printf("----------------------------\n");

    while(r >= 0 && r < size && c >= 0 && c < size) {
        idx = r * size + c;
        val = board[idx];
        steps++;

        if(val == 1) r--;      // N
        else if(val == 2) c++; // E
        else if(val == 3) r++; // S
        else if(val == 4) c--; // V

        board[idx] = (val % 4) + 1;
    }

    printf("Tabla %dx%d terminata dupa %d pasi.\n", size, size, steps);
    printf("Configuratia finala a tablei:\n");
    print_board(board, size);
    printf("----------------------------\n");
}

int main(int argc, char *argv[]) {
    next = uptime(); 
    
    printf("--- Simularea Fourway Problem (xv6) ---\n");
    simulate(3);
    simulate(5);

    exit(0);
}
