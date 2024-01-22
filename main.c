#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") // Vincular con la biblioteca de Winsock

#define SERVER_IP "127.0.0.1" // Reemplazar con la IP del servidor
#define PORT 20000
#define BUFFER_SIZE 1024

void uploadFile(SOCKET sock, const char *filename);

int main() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server_addr;

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        exit(EXIT_FAILURE);
    }

    // Crear socket del cliente
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        perror("socket failed");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Configurar dirección del servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // Conectar al servidor FTP
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect failed");
        closesocket(sock);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Subir un archivo
    uploadFile(sock, "fichSubida.txt"); // Reemplaza con el path correcto

    // Cerrar el socket
    closesocket(sock);
    WSACleanup();

    return 0;
}

void uploadFile(SOCKET sock, const char *filename) {
    char buffer[BUFFER_SIZE];
    FILE *file;
    int bytesRead;

    // Enviar comando de subida
    sprintf(buffer, "U %s", filename);
    send(sock, buffer, strlen(buffer), 0);

    // Abrir el archivo
    file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return;
    }

    // Leer y enviar el contenido del archivo
    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(sock, buffer, bytesRead, 0);
    }

    // Cerrar el archivo
    fclose(file);

    // Leer la respuesta del servidor
    recv(sock, buffer, BUFFER_SIZE, 0);
    printf("Respuesta del servidor: %s\n", buffer);
}
