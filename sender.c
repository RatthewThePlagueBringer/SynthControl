#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/syscall.h>

// System call IDs
#define PRINT_MSG 435
#define APPEND_INT 436
#define APPEND_LONG 437
#define APPEND_STRING 438
#define APPEND_CHAR 439
#define APPEND_COLOR 440
#define APPEND_BOOL 441
#define APPEND_NIL 442
#define APPEND_INFINITUM 443
#define APPEND_ARRAY_START 444
#define APPEND_ARRAY_END 445
#define ADD_META 446
#define SEND_MSG 447

#define BUFFER_SIZE 1024
#define OSC_BUFFER_SIZE 8

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <receiver_ip> <receiver_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in receiver_addr;
    memset(&receiver_addr, 0, sizeof(receiver_addr));
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(atoi(argv[2]));
    
    if (inet_pton(AF_INET, argv[1], &receiver_addr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    char input[BUFFER_SIZE];
    char message[BUFFER_SIZE] = {0};
    size_t msg_len = 0;
    ssize_t bytes_sent = 0;

    printf("Usage: \n\tclear - clear the buffer\n\tcommit - send the message\n\tadd meta <num> - add metadata for <num> arguments\n\tappend int <num> - append an integer\n\tappend string <str> - append a string\n\t/quit - exit the program\n\t/kill - close listener\n");

    // Command processing loop
    while (1) {
        printf("> ");
        if (!fgets(input, BUFFER_SIZE, stdin)) break;
        input[strcspn(input, "\n")] = 0;  // Remove newline

        // Close sender
        if (strcmp(input, "/quit") == 0) {
            printf("Quitting...\n");
            break;
        }

        // Process commands
        if (strcmp(input, "clear") == 0) {
            message[0] = '\0';
            msg_len = 0;
            printf("Buffer cleared.\n");
        } 
        else if (strcmp(input, "commit") == 0) {
            if (msg_len > 0) {
                int ret = syscall(SEND_MSG, argv[1], argv[2], message, msg_len);
                bytes_sent = sendto(sockfd, message, msg_len, 0, (const struct sockaddr *)&receiver_addr, sizeof(receiver_addr));
                printf("Sent: %s\n", message);
            } else {
                printf("Buffer empty!\n");
            }
        }
        else if (strncmp(input, "add meta ", 9) == 0) {
            char *num_str = input + 9;
            char *end;
            long num = strtol(num_str, &end, 10);

            int ret = syscall(ADD_META, message, OSC_BUFFER_SIZE, num);
            msg_len += 8;
        }
        else if (strncmp(input, "append int ", 11) == 0) {
            char *num_str = input + 11;
            char *end;
            long num = strtol(num_str, &end, 10);
            
            if (*end != '\0') {
                printf("Invalid integer: %s\n", num_str);
                continue;
            }
            
            int space_needed = msg_len > 0 ? 1 : 0;
            int needed = strlen(num_str) + space_needed;
            
            if (msg_len + needed >= BUFFER_SIZE) {
                printf("Buffer full!\n");
                continue;
            }
            
            if (space_needed) message[msg_len++] = ' ';
            int ret = syscall(APPEND_INT, message, num);
            msg_len += sizeof(num);
        }
        else if (strncmp(input, "append string ", 14) == 0) {
            char *str = input + 14;
            int space_needed = msg_len > 0 ? 1 : 0;
            int needed = strlen(str) + space_needed;
            
            if (msg_len + needed >= BUFFER_SIZE) {
                printf("Buffer full!\n");
                continue;
            }
            
            if (space_needed) message[msg_len++] = ' ';
            int ret = syscall(APPEND_STRING, message, str);
            msg_len += strlen(str);
        }
        else {
            printf("Unknown command\n");
        }
    }

    printf("Sent %zd bytes to %s:%s\n", bytes_sent, argv[1], argv[2]);
    close(sockfd);
    return 0;
}
