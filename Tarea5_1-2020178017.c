#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define NUM_MAX 15
// PID de subprocesos
pid_t pidPar, pidImpar;

// handler de señales vacío para atraparlas
void handler(int sig) {}

int main() {
    // iniciar handlers
    signal(SIGUSR1, handler);
    signal(SIGUSR2, handler);

    // inciar subproceso 1
    pidPar = fork();
    if (pidPar == 0) {
        signal(SIGUSR1, handler);
        for (int i = 0; i <= NUM_MAX; i += 2) { // para cada número par
            pause(); // esperar señal del subproceso impar
            printf("Subproceso 1 - Pares: %d\n", i);
            kill(getppid(), SIGUSR2); // enviar señal al proceso padre
        }
        exit(0); // salir después de terminar
    }
    // inciar subproceso 2
    pidImpar = fork();
    if (pidImpar == 0) {
        signal(SIGUSR2, handler);
        for (int i = 1; i <= NUM_MAX; i += 2) { // para cada número impar
            pause(); // esperar señal del proceso par
            printf("Subproceso 2 - Impares: %d\n", i);
            kill(getppid(), SIGUSR1); // señal al proceso padre
        }
        exit(0); // salir después de terminar
    }

    // proceso padre inicia subproceso 1
    kill(pidPar, SIGUSR1);

    // el padre se encarga de enviar señales a los hijos
    for (int i = 0; i < NUM_MAX; i++) {
        pause(); // espera señal de hijo
        if (i % 2 == 0) {
            kill(pidImpar, SIGUSR2); // par, hijo 2 continua
        } else {
            kill(pidPar, SIGUSR1); // impar, hijo 1 continua
        }
    }

    wait(NULL);
    wait(NULL);

    return 0;
}