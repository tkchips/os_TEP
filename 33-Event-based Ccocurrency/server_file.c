#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define PORT 65436
#define BUFFER_SIZE 1024

void get_current_time(char *buffer) {
    time_t raw_time;
    struct tm *time_info;

    time(&raw_time);
    time_info = localtime(&raw_time);

    strftime(buffer, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S\n\t", time_info);
}

int check_and_open_file(const char* filename) {
    printf("finding... : [%s]\n", filename);
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("file not found");
    }
    return fd;
}



char read_buffer[1025];

#define MAX_CLIENT 10

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    int fd;
    ssize_t read_count;

    int max_fd, activity;
    fd_set read_fds;
    char clients[MAX_CLIENT] = {0};
    int client_count = 0;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    int on = 1;
    int ret = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

     while (1) {
        FD_ZERO(&read_fds);

        FD_SET(server_fd, &read_fds);
        max_fd = server_fd;

        for (int i = 0; i < MAX_CLIENT; i++) {
            int sd = clients[i];
            if (sd > 0) {
                FD_SET(sd, &read_fds);
            }
            if (sd > max_fd) {
                max_fd = sd;
            }
        }
        activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("Select error");
        }

        if (FD_ISSET(server_fd, &read_fds)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }

            printf("新客户端连接, socket fd: %d, IP: %s, Port: %d\n",
                   new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            for (int i = 0; i < MAX_CLIENT; i++) {
                if (clients[i] == 0) {
                    clients[i] = new_socket;
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENT; i++) {
            int sd = clients[i];

            if (FD_ISSET(sd, &read_fds)) {
                memset(buffer, 0, BUFFER_SIZE);

                int valread = read(sd, buffer, BUFFER_SIZE);
                if (valread == 0) {
                    printf("客户端断开连接, socket fd: %d\n", sd);
                    close(sd);
                    clients[i] = 0;
                } else {
                    printf("收到客户端消息: %s", buffer);

                    char* p = strchr(buffer, '\n');
                    *p = '\0';

                    fd = check_and_open_file(buffer);
                    if (fd != -1) {
                        memset(read_buffer, 0, sizeof(read_buffer));
                        while ((read_count = read(fd, read_buffer, 1024)) > 0) {
                            send(sd, read_buffer, read_count, 0);
                            read_buffer[read_count] = '\0';
                            printf("%s", read_buffer);
                            memset(read_buffer, 0, sizeof(read_buffer));
                        }
                        printf("\n");
                        close(fd);
                    } else {
                        const char *error_msg = "File is not found.\n";
                        send(sd, error_msg, strlen(error_msg), 0);
                    }
                }
            }
        }
    }

    return 0;
}