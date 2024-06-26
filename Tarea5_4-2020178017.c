#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void send_file(FILE *fp, int sockfd) {
    char data[BUFFER_SIZE] = {0};

    while (fgets(data, BUFFER_SIZE, fp) != NULL) {
        if (send(sockfd, data, sizeof(data), 0) == -1) {
            perror("Error al enviar el archivo");
            exit(1);
        }
        bzero(data, BUFFER_SIZE);
    }
}

int main() {
    int sockfd, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    char buffer[BUFFER_SIZE];
    FILE *fp;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error al crear el socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al realizarse el bind");
        exit(1);
    }

    if (listen(sockfd, 10) != 0) {
        perror("Error al escuchar el puerto");
        exit(1);
    }

    addr_size = sizeof(new_addr);
    new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
    if (new_sock < 0) {
        perror("Error al aceptar la conexión");
        exit(1);
    }

    // Recibir el nombre del archivo del cliente
    recv(new_sock, buffer, BUFFER_SIZE, 0);
    printf("El cliente solicitó el archivo: %s\n", buffer);

    fp = fopen(buffer, "r");
    if (fp == NULL) {
        perror("Error al abrir el archivo solicitado");
        exit(1);
    }

    send_file(fp, new_sock);
    printf("Archivo enviado satisfactoriamente al cliente \n");

    close(new_sock);
    close(sockfd);

    return 0;
}
