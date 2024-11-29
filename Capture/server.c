#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define LOG_FILE "network_devices.log"

// Function to check if an entry is unique
bool is_unique_entry(const char *entry) {
    FILE *file = fopen(LOG_FILE, "r");
    if (!file) return true;

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, entry)) {
            fclose(file);
            return false; 
        }
    }
    fclose(file);
    return true;
}

// Function to log to a file
void log_to_file(const char *entry) {
    if (is_unique_entry(entry)) {
        FILE *file = fopen(LOG_FILE, "a");
        if (!file) {
            perror("File open error");
            return;
        }
        fprintf(file, "%s", entry);
        fclose(file);
        printf("Logged: %s", entry);
    } else {
        printf("Duplicate entry ignored: %s", entry);
    }
}

void start_server(int server_port) {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    char buffer[1024];
    socklen_t addr_len = sizeof(client_addr);

    // Create a socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to IP and port
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind error");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(server_sock, 5) < 0) {
        perror("Listen error");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on 0.0.0.0:%d\n", server_port);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len);
        if (client_sock < 0) {
            perror("Accept error");
            continue;
        }

        printf("Connection established with client %s\n", inet_ntoa(client_addr.sin_addr));

        while (1) {
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
            if (bytes_received <= 0) break;

            buffer[bytes_received] = '\0';
            if (strcmp(buffer, "END\n") == 0) break;

            log_to_file(buffer);
        }

        printf("Connection closed.\n");
        close(client_sock);
    }

    close(server_sock);
}

int main() {
    int server_port = 5000;
    start_server(server_port);
    return 0;
}
