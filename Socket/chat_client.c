#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

void *receive_messages(void *socket_desc) {
    int client_socket = *(int *)socket_desc;
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received > 0) {
            printf("%s", buffer);
        }
    }

    pthread_exit(NULL);
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("Connected to the server.\n");

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, &client_socket);

    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);
        send(client_socket, buffer, strlen(buffer), 0);
    }

    close(client_socket);
    return 0;
}
