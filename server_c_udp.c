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
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[129];

    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd < 0) 
        error("ERROR opening socket");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) 
        error("ERROR on binding");

    client_len = sizeof(client_addr);
    
    while (1) {
        ssize_t bytes_received = recvfrom(server_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_len);
        if (bytes_received <= 0) {
            continue; 
        }
        buffer[bytes_received] = '\0';
        buffer[strcspn(buffer, "\r\n")] = '\0';

        if (!is_all_numbers(buffer)) {
            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "%s", "Sorry, cannot compute!\n");
            sendto(server_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, client_len);
        }
        else {
            int sum = 0;
            buffer[bytes_received] = '\0';
            buffer[strcspn(buffer, "\r\n")] = '\0';
            char result[129] = "";
            do {
                sum = get_sum(buffer);
                char temp[32];
                sprintf(temp, "%d\n", sum);
                strcat(result, temp);
                sprintf(buffer, "%d", sum);
            } while (sum >= 10);

            sendto(server_fd, result, strlen(result), 0,
                   (struct sockaddr *)&client_addr, client_len);
        }
    }

    close(server_fd);
    return 0;
}

