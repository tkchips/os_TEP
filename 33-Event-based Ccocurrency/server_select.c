#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 65436
#define BUFFER_SIZE 1024

void get_current_time(char *buffer) {
    time_t raw_time;
    struct tm *time_info;

    time(&raw_time);
    time_info = localtime(&raw_time);

    strftime(buffer, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S\n\t", time_info);
}

#define MAX_CLIENT 10

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

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
        // 清空文件描述符集合
        FD_ZERO(&read_fds);

        // 将服务器套接字加入集合
        FD_SET(server_fd, &read_fds);
        max_fd = server_fd;

        // 将客户端套接字加入集合
        for (int i = 0; i < MAX_CLIENT; i++) {
            int sd = clients[i];
            if (sd > 0) {
                FD_SET(sd, &read_fds);
            }
            if (sd > max_fd) {
                max_fd = sd;
            }
        }

        // 等待活动的文件描述符
        activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("Select error");
        }

        // 如果服务器套接字有活动，说明有新的连接请求
        if (FD_ISSET(server_fd, &read_fds)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }

            printf("新客户端连接，socket fd: %d, IP: %s, Port: %d\n",
                   new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // 将新连接的客户端套接字加入数组
            for (int i = 0; i < MAX_CLIENT; i++) {
                if (clients[i] == 0) {
                    clients[i] = new_socket;
                    break;
                }
            }
        }

        // 检查每个客户端套接字是否有活动
        for (int i = 0; i < MAX_CLIENT; i++) {
            int sd = clients[i];

            if (FD_ISSET(sd, &read_fds)) {
                // 清空缓冲区
                memset(buffer, 0, BUFFER_SIZE);

                // 读取客户端消息
                int valread = read(sd, buffer, BUFFER_SIZE);
                if (valread == 0) {
                    // 客户端断开连接
                    printf("客户端断开连接，socket fd: %d\n", sd);
                    close(sd);
                    clients[i] = 0;
                } else {
                    printf("收到客户端消息: %s", buffer);

                    // 判断是否收到 "time\n"
                    if (strncmp(buffer, "time", 4) == 0) {
                        // 获取当前时间
                        get_current_time(buffer);

                        // 发送当前时间给客户端
                        send(sd, buffer, strlen(buffer), 0);
                        printf("已发送时间: %s\n", buffer);
                    } else {
                        // 如果收到的消息不是 "time\n"，发送错误消息
                        const char *error_msg = "Invalid command. Please send 'time'.\n";
                        send(sd, error_msg, strlen(error_msg), 0);
                    }
                }
            }
        }
    }

    return 0;
}