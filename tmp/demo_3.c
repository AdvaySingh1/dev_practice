#include <fcntl.h>    // open()
#include <unistd.h>   // close(), sleep(), ftruncate()
#include <sys/mman.h> // mmap(), msync(), munmap()
#include <sys/stat.h> // fstat()
#include <string.h>   // memset(), strlen()
#include <stdio.h>    // snprintf()

int main()
{
    const char *path = "somefile.txt";
    const size_t MAPLEN = 4096; // map one page

    // Open read/write so the mapping can be writable
    int fd = open(path, O_RDWR | O_CREAT, 0644);
    if (fd < 0)
        return 1;

    // Ensure file is at least MAPLEN bytes long
    if (ftruncate(fd, MAPLEN) < 0)
        return 1;

    // Map file into memory (shared so changes go to the file/page cache)
    char *buf = mmap(NULL, MAPLEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (buf == MAP_FAILED)
        return 1;

    // No longer need the fd for writing once mapped (optional to keep it open)
    // close(fd);

    int i = 0;
    while (1)
    {
        // Overwrite the mapped region with a message + counter
        memset(buf, 0, MAPLEN);
        int n = snprintf(buf, MAPLEN, "Hello, world via mmap! count=%d\n", i++);

        // Flush changes to disk (optional; kernel will flush eventually)
        msync(buf, (size_t)n, MS_ASYNC);

        sleep(1);
    }

    // Unreachable in this loop, but good practice:
    munmap(buf, MAPLEN);
    close(fd);
    return 0;
}
