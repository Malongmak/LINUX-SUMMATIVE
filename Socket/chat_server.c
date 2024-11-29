#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 4
#define BUFFER_SIZE 1024

typedef struct {
    int socket;
    char username[50];
    int authenticated;
} Client;

Client clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;


void send_message_to_client(int sender_index, const char *recipient, const char *message) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].authenticated && strcmp(clients[i].username, recipient) == 0) {
            char formatted_message[BUFFER_SIZE];
            snprintf(formatted_message, BUFFER_SIZE, "Message from %s: %s\n", clients[sender_index].username, message);
            send(clients[i].socket, formatted_message, strlen(formatted_message), 0);
            pthread_mutex_unlock(&clients_mutex);
            return;
        }
    }

    char error_message[] = "Recipient not found or not authenticated.\n";
    send(clients[sender_index].socket, error_message, strlen(error_message), 0);
    pthread_mutex_unlock(&clients_mutex);
}

void broadcast_online_clients(int sender_socket) {
    char online_list[BUFFER_SIZE] = "Online clients: ";
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].authenticated) {
            strcat(online_list, clients[i].username);
            strcat(online_list, ", ");
        }
    }

    pthread_mutex_unlock(&clients_mutex);
    strcat(online_list, "\n");
    send(sender_socket, online_list, strlen(online_list), 0);
}


void *handle_client(void *arg) {
    int client_index = *(int *)arg;
    int client_socket = clients[client_index].socket;
    char buffer[BUFFER_SIZE];

    send(client_socket, "Enter your username: ", 21, 0);
    recv(client_socket, clients[client_index].username, 50, 0);
    clients[client_index].username[strcspn(clients[client_index].username, "\n")] = '\0'; 
    clients[client_index].authenticated = 1;

    send(client_socket, "Authenticated successfully!\n", 28, 0);
    broadcast_online_clients(client_socket);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            break;
        }

        char recipient[50];
        char message[BUFFER_SIZE];

        sscanf(buffer, "%s %[^\n]", recipient, message);
        send_message_to_client(client_index, recipient, message);
    }

    pthread_mutex_lock(&clients_mutex);
    clients[client_index].authenticated = 0;
    clients[client_index].socket = 0;
    pthread_mutex_unlock(&clients_mutex);

    close(client_socket);
    pthread_exit(NULL);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, MAX_CLIENTS);

    printf("Server is running on port 8080...\n");

    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].socket = 0;
        clients[i].authenticated = 0;
    }

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);

        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].socket == 0) {
                clients[i].socket = client_socket;
                pthread_t thread;
                pthread_create(&thread, NULL, handle_client, &i);
                pthread_detach(thread);
                break;
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }

    close(server_socket);
    return 0;
}
