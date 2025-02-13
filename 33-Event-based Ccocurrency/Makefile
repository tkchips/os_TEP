CC = gcc

CFLAGS = -Wall -pthread -O2

ALL = server server_select server_file client_file

all: $(ALL)

clean:
	rm -f $(ALL) *~

server: server.c
	$(CC) -o server server.c $(CFLAGS)

server_select: server_select.c
	$(CC) -o server_select server_select.c $(CFLAGS)

server_file: server_file.c
	$(CC) -o server_file server_file.c $(CFLAGS)

client_file: client_file.c
	$(CC) -o client_file client_file.c $(CFLAGS)



# 自动推导规则：.c 文件生成 .o 文件
%.o: %.c
	$(CC) -c $< $(CFLAGS)

# Phony 目标声明
.PHONY: all clean