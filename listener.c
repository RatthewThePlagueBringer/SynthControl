#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/syscall.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <listen_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Allow address reuse
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %s...\n", argv[1]);
    
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        char buffer[BUFFER_SIZE];
        
        ssize_t bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                     (struct sockaddr *)&client_addr, &client_len);
        
        if (bytes_received < 0) {
            perror("Receive failed");
            continue;
        }

        buffer[bytes_received] = '\0';
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

        if (strcmp(buffer, "/quit") == 0) {
            printf("Received /quit command. Exiting.\n");
            break;
        }
        
        printf("Message: \n");
        for (int i = 0; i < bytes_received; i++) {
		    printf("%c (%d)\t", buffer[i], buffer[i]);
		    if (!((i + 1) % 4)) putchar('\n');
	    }
        putchar('\n');
    }

    close(sockfd);
    return 0;
}
