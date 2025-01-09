#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>

#define DEVICES_NUM 6
#define DEVICE_PREFIX "/dev/shofer"

void closeDevices();
struct pollfd fds[DEVICES_NUM];

int main() {
    signal(SIGINT, closeDevices);
    char buffer[64];
    int i, ret;

    // Open all devices for reading
    for (i = 0; i < DEVICES_NUM; i++) {
        char device_name[32];
        snprintf(device_name, sizeof(device_name), "%s%d", DEVICE_PREFIX, i);
        fds[i].fd = open(device_name, O_RDONLY);
        if (fds[i].fd < 0) {
            perror("open");
            return EXIT_FAILURE;
        }
        fds[i].events = POLLIN;
    }

    printf("Reader program: Waiting for data...\n");

    while (1) {
        // Wait for data on any device
        ret = poll(fds, DEVICES_NUM, -1);
        if (ret < 0) {
            perror("poll");
            break;
        }

        for (i = 0; i < DEVICES_NUM; i++) {
            if (fds[i].revents & POLLIN) {
                // Data is available, read it
                ssize_t n = read(fds[i].fd, buffer, 1);
                if (n > 0) {
                    buffer[n] = '\0';
                    printf("Device %d: %s\n", i, buffer);
                } else {
                    perror("read");
                }
            }
        }
    }

    return EXIT_SUCCESS;
}

void closeDevices() {
    // Close all devices
    for (int i = 0; i < DEVICES_NUM; i++) {
        close(fds[i].fd);
    }
    exit(0);
}
