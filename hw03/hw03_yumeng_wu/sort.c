
#include <unistd.h>
#include <stdio.h> // for perror
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void has_error(int n)
{
    if (n < 0)
    {
        perror("");
        exit(1);
    }
}

void merge_sort(int * array, long left, long right)
{
    if (left >= right || left + 1 == right)
    {
        return;
    }
    long mid = left + (right - left) / 2;
    int lLen = mid - left, rLen = right - mid;
    int lArr[lLen], rArr[rLen];
    for (int i = 0; i < lLen; ++i)
    {
        lArr[i] = array[left + i];
    }
    for (int i = 0; i < rLen; ++i)
    {
        rArr[i] = array[mid + i];
    }
    merge_sort(lArr, 0, lLen);
    merge_sort(rArr, 0, rLen);
    int i = 0, j = 0, k = left;
    while (i < lLen || j < rLen)
    {
        if (j == rLen)
            array[k++] = lArr[i++];
        else if (i == lLen)
            array[k++] = rArr[j++];
        else if (lArr[i] > rArr[j])
            array[k++] = rArr[j++];
        else
            array[k++] = lArr[i++];
    }
}

void sort(int * array, long nn)
{
    merge_sort(array, 0, nn);
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("Usage: ./sort input-file output-file\n");
        return 1;
    }
    int fd, res;
    long len;
    fd = open(argv[1], O_RDONLY);
    has_error(fd);
    off_t fstart = lseek(fd, 0, SEEK_CUR);
    has_error(fstart);
    off_t fend = lseek(fd, 0, SEEK_END);
    has_error(fend);
    len = (fend - fstart) / 4;
    int array[len];
    res = lseek(fd, 0, SEEK_SET);
    has_error(res);
    for (int i = 0; i < len; ++i)
    {
        res = read(fd, &array[i], 4);
        has_error(res);
    }
    close(fd);
    sort(array, len);
    fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC);
    for (int i = 0; i < len; ++i)
    {
        res = write(fd, &array[i], 4);
        has_error((res == 4) - 1);
    }
    close(fd);
    return 0;
}
