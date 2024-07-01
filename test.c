//
// Created by poti on 01/07/24.
//

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {

    printf("PID: %d\n", getpid());
    int fd = open("/dev/customdevice", O_RDONLY);
    if (fd <= 2) {
        perror("Driver open error");
        return 1;
    }
    printf("Device driver opened on fd %d\n", fd);
    if (ioctl(fd, 0) != 0) perror("Ioctl error");
    if(close(fd) != 0) perror("Device closing error");
}