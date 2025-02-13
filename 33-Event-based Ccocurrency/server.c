#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 65433
#define BUFFER_SIZE 1024

void get_current_time(char *buffer) {
    time_t raw_time;
    struct tm *time_info;

    time(&raw_time);
    time_info = localtime(&raw_time);

    strftime(buffer, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S\n\t", time_info);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

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
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        printf("已连接来自 %s:%d 的客户端\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

        while (1) {
            memset(buffer, 0, BUFFER_SIZE);

            int valread = read(new_socket, buffer, BUFFER_SIZE);
            if (valread <= 0) {
                printf("客户端断开连接或发生错误\n");
                break;
            }

            printf("收到客户端消息: %s", buffer);

            if (strncmp(buffer, "time", 4) == 0) {
                get_current_time(buffer);

                send(new_socket, buffer, strlen(buffer), 0);
                printf("已发送时间: %s\n", buffer);
            } else {
                const char *error_msg = "Invalid command. Please send 'time'.\n";
                send(new_socket, error_msg, strlen(error_msg), 0);
            }
        }

        // 关闭客户端连接
        close(new_socket);
    }

    return 0;
}