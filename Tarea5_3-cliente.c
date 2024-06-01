#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void *receive_messages(void *socket_desc) {
    int sock = *(int *)socket_desc;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while ((bytes_received = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);
    }

    return NULL;
}

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char message[BUFFER_SIZE];
    pthread_t receive_thread;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en la conexión");
        close(sock);
        exit(EXIT_FAILURE);
    }

    pthread_create(&receive_thread, NULL, receive_messages, (void *)&sock);

    while (1) {
        fgets(message, BUFFER_SIZE, stdin);
        if (send(sock, message, strlen(message), 0) < 0) {
            perror("Error al enviar el mensaje");
            break;
        }
    }

    close(sock);
    return 0;
}
