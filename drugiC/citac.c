#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define RED "/dev/shofer"
#define MAXSZ 64

int main() {
    int fp;
    char buffer[MAXSZ];

    fp = open(RED, O_RDONLY);
    if (fp == -1) {
        perror("Open failed");
        return 1;
    }

    while (1) {
        memset(buffer, 0, MAXSZ);
        if (read(fp, buffer, MAXSZ) > 0) {
            printf("Read message: %s\n", buffer);
        }
        sleep(1);
    }

    close(fp);
    return 0;
}
