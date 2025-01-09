#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>
#include <signal.h>

#define DEVICES_NUM 6
#define DEVICE_PREFIX "/dev/shofer"

void closeDevices();
struct pollfd fds[DEVICES_NUM];

int main() {
    signal(SIGINT, closeDevices);
    char data = 'A'; // Initial data to write
    int i, ret;

    // Seed random number generator
    srand(time(NULL));

    // Open all devices for writing
    for (i = 0; i < DEVICES_NUM; i++) {
        char device_name[32];
        snprintf(device_name, sizeof(device_name), "%s%d", DEVICE_PREFIX, i);
        fds[i].fd = open(device_name, O_WRONLY);
        if (fds[i].fd < 0) {
            perror("open");
            return EXIT_FAILURE;
        }
        fds[i].events = POLLOUT;
    }

    printf("Writer program: Periodically checking for writable devices...\n");

    while (1) {
        // Wait for writable devices (5 seconds timeout)
        ret = poll(fds, DEVICES_NUM, 5000);
        if (ret < 0) {
            perror("poll");
            break;
        } else if (ret == 0) {
            printf("No writable devices available, retrying...\n");
            continue;
        }

        int selected_device = rand() % DEVICES_NUM;
        if (fds[selected_device].revents & POLLOUT) {
            // Device is writable, send data
            ssize_t n = write(fds[selected_device].fd, &data, 1);
            if (n > 0) {
                printf("Wrote '%c' to device %d\n", data, selected_device);
                data++; // Increment data for the next write
            } else {
                perror("write");
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
