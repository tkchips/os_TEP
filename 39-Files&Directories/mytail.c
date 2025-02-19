#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>   
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[]) {
    struct stat sb;
    int fd, offset, lines;
    char *pathname = NULL;

    if (argc != 3 || strlen(argv[1]) <= 1) {
        fprintf(stderr, "Usage: %s -<lines> <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    lines = atoi(argv[1] + 1);
    pathname = argv[2];

    if (stat(pathname, &sb) == -1)
        handle_error("stat");

    if ((fd = open(pathname, O_RDONLY)) == -1)
        handle_error("open");

    if (sb.st_size == 0) {
        printf("File is empty.\n");
        close(fd);
        exit(EXIT_SUCCESS);
    }

    offset = sb.st_size - 1; 
    if (lseek(fd, offset, SEEK_SET) == -1)
        handle_error("lseek");

    char buffer[4096];
    ssize_t bytes_read = 0;
    int newline_count = 0;

    while (offset >= 0 && newline_count < lines) {
        if (lseek(fd, offset, SEEK_SET) == -1)
            handle_error("lseek");

        if (read(fd, buffer, 1) != 1)
            handle_error("read");

        if (buffer[0] == '\n') {
            newline_count++;
        }

        offset--;
    }

    if (offset < 0) {
        offset = 0;
    } else {
        offset += 2;
    }

    if (lseek(fd, offset, SEEK_SET) == -1)
        handle_error("lseek");

    memset(buffer, 0, sizeof(buffer));
    bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read == -1)
        handle_error("read");

    buffer[bytes_read] = '\0';
    printf("--BEGIN--\n%s\n--EOF--\n", buffer);

    close(fd);
    exit(EXIT_SUCCESS);
}