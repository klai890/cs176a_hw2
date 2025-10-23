#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <ctype.h> // for isdigit()

void error(const char *msg){
    perror(msg);
    exit(0);
}

bool is_all_numbers(char* str){
    if (str == NULL || *str == '\0') {
        return false;
    }

    while (*str != '\0') {
        if (!isdigit((unsigned char)*str)) {
            return false;
        }
        str++;
    }

    return true;
}

int get_sum(char* str){
    int result = 0;
    while (*str != '\0') {
        result += *str - '0';
        str++;
    }

    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    int port = atoi(argv[1]);
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[128];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) 
        error("ERROR opening socket");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) 
        error("ERROR on binding");

    listen(server_fd, 5);
    client_len = sizeof(client_addr);
    
    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

        if (client_fd < 0) {
            error("ERROR on accept");
            continue; 
        }

        ssize_t bytes_received = recv(client_fd, buffer, 127, 0);
        if (bytes_received <= 0) {
            close(client_fd);
            continue; 
        }
        buffer[bytes_received] = '\0';
        buffer[strcspn(buffer, "\r\n")] = '\0';
        char* cannot_compute = "Fron server: Sorry, cannot compute!\n";
        if (!is_all_numbers(buffer)) {
            send(client_fd, cannot_compute, strlen(cannot_compute), 0);
        }
        else {
            int sum = 0;
            do {
                sum = get_sum(buffer);
                sprintf(buffer, "%d", sum);
                char send_msg[128];
                sprintf(send_msg, "From server: %d\n", sum);
                send(client_fd, send_msg, strlen(send_msg), 0);
            } while (sum >= 10);
        }

        close(client_fd); 

    }

    close(server_fd);
    return 0;
}

