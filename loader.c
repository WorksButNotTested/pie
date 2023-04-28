#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef int (*func)(void);

int main(int argc, char **argv)
{
    int ret = -1;
    int r = -1;
    char *image = NULL;
    int fd = -1;
    void *map = MAP_FAILED;
    size_t size = 0;
    struct stat stats = {0};
    func target = NULL;
    do
    {
        puts("LOADER");
        if (argc < 2)
        {
            printf("usage: %s <image>\n", argv[0]);
            break;
        }

        image = argv[1];

        printf("Loading %s...\n", image);

        fd = open(image, O_RDONLY);
        if (fd < 0)
        {
            printf("Failed to open image: %d\n", errno);
            break;
        }

        r = fstat(fd, &stats);
        if (r < 0)
        {
            printf("Failed to fstat: %d (%d)\n", r, errno);
            break;
        }

        size = stats.st_size;

        printf("Image is %d bytes\n", size);

        map = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, fd, 0);
        // map = mmap((void *)0x80000000, size, PROT_READ | PROT_WRITE | PROT_EXEC,
        //            MAP_PRIVATE | MAP_FIXED | MAP_FIXED_NOREPLACE, fd, 0);
        if (map == MAP_FAILED)
        {
            printf("Failed to mmap: (%d)\n", errno);
            break;
        }

        printf("Image is mapped at %p\n", map);

        target = (func)map;
        r = target();
        printf("r: 0x%08x\n", r);

        ret = 0;
    } while (false);

    if (map != MAP_FAILED)
    {
        munmap(map, size);
    }

    if (fd != -1)
    {
        close(fd);
    }

    return ret;
}
