#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void write_file(int sockfd) {
    int n;
    FILE *fp;
    // El programa solo funciona si se solicitan otros archivos .txt
    char *filename = "archivo_recibido.txt";
    char buffer[BUFFER_SIZE];

    fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Error al crear el archivo");
        exit(1);
    }

    while (1) {
        n = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (n <= 0) {
            break;
        }
        fprintf(fp, "%s", buffer);
        bzero(buffer, BUFFER_SIZE);
    }
    fclose(fp);
}

int main() {
    int sockfd, ret;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char filename[BUFFER_SIZE];

    printf("Digite el nombre del archivo que desea obtener: ");
    scanf("%s", filename);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error al crear el socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    ret = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret == -1) {
        perror("Error en la conexión");
        exit(1);
    }

    // Enviar el nombre del archivo al servidor, debe ser solo .txt 
    send(sockfd, filename, strlen(filename), 0);

    write_file(sockfd);
    printf("Archivo recibido exitosamente \n");

    close(sockfd);

    return 0;
}
