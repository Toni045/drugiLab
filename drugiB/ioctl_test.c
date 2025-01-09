#include <stdio.h>       // For perror
#include <fcntl.h>       // For open
#include <unistd.h>      // For close
#include <sys/ioctl.h>   // For ioctl
#include "config.h"      // Include ioctl definitions (ensure this is in your source folder)

int main() {
    int fd;
    struct shofer_ioctl cmd = {
        .command = SHOFER_IOCTL_COPY,
        .count = 10  // Number of bytes to transfer
    };

    fd = open("/dev/shofer_control", O_RDWR);
    if (fd < 0) {
        perror("Failed to open control device");
        return 1;
    }

    if (ioctl(fd, _IOWR(SHOFER_IOCTL_TYPE, SHOFER_IOCTL_NR, struct shofer_ioctl), &cmd) < 0) {
        perror("Failed to send ioctl command");
        close(fd);
        return 1;
    }

    printf("IOCTL command sent successfully.\n");

    close(fd);
    return 0;
}
