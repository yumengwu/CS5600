#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <fcntl.h>


int
main(int _ac, char* _av[])
{
    struct stat info;

    printf("Sizeof stat: %ld\n", sizeof(info));

    ptrdiff_t a0 = (ptrdiff_t) &info;
    ptrdiff_t a1 = (ptrdiff_t) &(info.st_size);

    printf("Offset of st_size: %ld\n", a1 - a0);

    printf("O_WRONLY | O_CREAT | O_TRUNC = 0%lo\n", O_WRONLY | O_CREAT | O_TRUNC);

    printf("SEEK_SET = %d\n", SEEK_SET);

    return 0;
}
