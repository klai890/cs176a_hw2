#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <ctype.h> // for isdigit()

// Print error message and exit
void error(const char *msg){
    perror(msg);
    exit(0);
}

// Determine if a string contains only digits
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

// Return sum of all digits in a string
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
    char buffer[129];

    // Socket setup
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) 
        error("ERROR opening socket");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) 
        error("ERROR on binding");

    // Listen for connections
    listen(server_fd, 5);
    client_len = sizeof(client_addr);
    
    while (1) {
        // Establish connection with client
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        
        if (client_fd < 0) {
            error("ERROR on accept");
            continue; 
        }

        // Process client message
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            close(client_fd);
            continue; 
        }

        // Ensure \0 at end
        buffer[bytes_received] = '\0';
        buffer[strcspn(buffer, "\n")] = '\0';

        if (!is_all_numbers(buffer)) {
            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "%s", "Sorry, cannot compute!\n");
            send(client_fd, buffer, strlen(buffer), 0);
        }

        else {
            int sum = 0;

            // Ensure \0 at end
            buffer[bytes_received] = '\0';
            buffer[strcspn(buffer, "\n")] = '\0';

            char result[129] = "";
            
            // Retrieve sum and store in final result
            do {
                sum = get_sum(buffer);
                char temp[6];
                sprintf(temp, "%d\n", sum);
                strcat(result, temp);
                sprintf(buffer, "%d", sum);
            } while (sum >= 10);
            
            send(client_fd, result, strlen(result), 0);
        }

        close(client_fd); 

    }

    close(server_fd);
    return 0;
}

