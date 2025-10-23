#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    socklen_t serv_len;

    char buffer[129];
    if (argc < 3) {
       fprintf(stderr,"Usage %s hostname port\n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET,SOCK_DGRAM, 0);

    if (sockfd < 0){
        error("ERROR opening socket");
    }

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    serv_len = sizeof(serv_addr);

    printf("Enter string: ");
    memset(buffer, 0, sizeof(buffer));
    fgets(buffer, sizeof(buffer), stdin);
    n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &serv_addr, serv_len);
    if (n < 0) {
        error("ERROR writing to socket");
    }

    // Read in from buffer
    memset(buffer, 0, sizeof(buffer));
    n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &serv_addr, &serv_len);
    if (n > 0) {
        printf("From server: ");
        for (int i = 0; i < n; i++) {
            if (buffer[i] == '\n' && i != n - 1) {
                printf("\nFrom server: ");
            }
            else {
                printf("%c", buffer[i]);
            }
        }
    }

    close(sockfd);
    return 0;
}   