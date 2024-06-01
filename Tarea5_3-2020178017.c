#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

int client_sockets[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast_message(char *message, int sender_socket) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (client_sockets[i] != sender_socket) {
            if (send(client_sockets[i], message, strlen(message), 0) < 0) {
                perror("Error el enviar el mensaje");
                continue;
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[bytes_received] = '\0';
        broadcast_message(buffer, client_socket);
    }

    close(client_socket);
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (client_sockets[i] == client_socket) {
            for (int j = i; j < client_count - 1; j++) {
                client_sockets[j] = client_sockets[j + 1];
            }
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    free(arg);
    pthread_exit(NULL);
}

int main() {
    int server_socket, new_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    pthread_t tid;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al realizar el bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) < 0) {
        perror("Error al escuchar el puerto");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Servidor activo en el puerto:  %d\n", PORT);

    while (1) {
        addr_size = sizeof(client_addr);
        new_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
        if (new_socket < 0) {
            perror("Error al aceptar el mensaje");
            continue;
        }

        pthread_mutex_lock(&clients_mutex);
        client_sockets[client_count++] = new_socket;
        pthread_mutex_unlock(&clients_mutex);

        int *new_sock = malloc(sizeof(int));
        *new_sock = new_socket;
        pthread_create(&tid, NULL, handle_client, (void *)new_sock);
    }

    close(server_socket);
    return 0;
}
