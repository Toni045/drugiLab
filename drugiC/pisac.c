#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define RED "/dev/shofer"
#define MAXSZ 64

int main() {
    int fp;
    char buffer[MAXSZ];
    srand(time(NULL));

    fp = open(RED, O_WRONLY);
    if (fp == -1) {
        perror("Open failed");
        return 1;
    }

    while (1) {
        size_t len = rand() % (MAXSZ - 1) + 1;
        memset(buffer, 0, MAXSZ);
        for (size_t i = 0; i < len; i++)
            buffer[i] = 'A' + (rand() % 26);
        if (write(fp, buffer, len) > 0) {
            printf("Written message: %s\n", buffer);
        }
        sleep(2);
    }

    close(fp);
    return 0;
}
