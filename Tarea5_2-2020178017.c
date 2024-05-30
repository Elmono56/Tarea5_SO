#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define NUM_CHILDREN 10
// array que almacena los hijos
pid_t children[NUM_CHILDREN];

// terminar al hijo 
void handler(int sig) {
    printf("bye %d\n", getpid());
    exit(0);
}

void create_children() {
    for (int i = 0; i < NUM_CHILDREN; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            // iniciar handler de señales del hijo
            signal(SIGTERM, handler);
            while (1) {
                printf("PID hijo: %d\n", getpid());
                sleep(2);
            }
        } else {
            // proceso padre
            children[i] = pid;
        }
    }
}

// mover aleatoriamente y reordenar los subprocesos
void shuffle(int *array, size_t n) {
    if (n > 1) {
        size_t i;
        for (i = 0; i < n - 1; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}


// funcion auxiliar que cancela los hijos
void send_termination_signals() {
    int indices[NUM_CHILDREN];
    for (int i = 0; i < NUM_CHILDREN; i++) {
        indices[i] = i;
    }

    // mezclar aleatoriamente los procesos en el array
    shuffle(indices, NUM_CHILDREN);

    for (int i = 0; i < NUM_CHILDREN; i++) {
        kill(children[indices[i]], SIGTERM);
        sleep(2);
    }
}

int main() {
    // inicializador para numeros aleatorios
    srand(time(NULL));
    // crear array de hijos
    create_children();
    sleep(3);
    // empezar a cancelar los hijos
    send_termination_signals();

    printf("\n Todos los hijos han sido cancelados");
    return 0;
}