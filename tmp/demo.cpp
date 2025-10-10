#include <fcntl.h>     // open()
#include <unistd.h>    // write(), close(), sleep()
#include <string.h>    // strlen()

int main() {
    int fd = open("somefile.txt", O_WRONLY | O_CREAT, 0644);
    if (fd < 0) return 1;

    const char *msg = "Hello, world!\n";
    while (1) {
        write(fd, msg, strlen(msg));
        sleep(1);  // delay 1 second
    }

    close(fd);
    return 0;
}
