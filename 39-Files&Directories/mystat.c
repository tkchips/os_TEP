#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>

char* file;

int arg_parser(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: mystat <file>\n");
        return -1;
    }
    file = argv[1];
    return 0;
}

int main(int argc, char *argv[]) {
    if (arg_parser(argc, argv) == 0) {
        struct stat buf;
        if (stat(file, &buf) == -1) {
            if (errno == ENOENT) {
                printf("file is not exist\n");
            } else {
                perror("stat fail");
            }
        } else {
            printf("file size: %ld\n", buf.st_size);
            printf("file mode: %o\n", buf.st_mode);
            printf("file uid: %d\n", buf.st_uid);
            printf("file gid: %d\n", buf.st_gid);
            printf("file atime: %ld\n", buf.st_atime);
            printf("file mtime: %ld\n", buf.st_mtime);
            printf("file ctime: %ld\n", buf.st_ctime);
            printf("file inode: %ld\n", buf.st_ino);
            printf("file dev: %lu\n", buf.st_dev);
            printf("file nlink: %\n", buf.st_nlink);
            printf("file blocks: %ld\n", buf.st_blocks);
            printf("file blksize: %ld\n", buf.st_blksize);
            printf("file rdev: %lu\n", buf.st_rdev);
        }
    } else {
        return -1;
    }
    return 0;
}