#include <unistd.h>
#include <stdio.h> // for perror
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* check if sys call has error */
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
    /* calculate subarray size */
    long mid = left + (right - left) / 2;
    int lLen = mid - left, rLen = right - mid;
    /* allocate space for subarrays */
    int lArr[lLen], rArr[rLen];
    /* copy subarray */
    for (int i = 0; i < lLen; ++i)
    {
        lArr[i] = array[left + i];
    }
    for (int i = 0; i < rLen; ++i)
    {
        rArr[i] = array[mid + i];
    }
    /* recursively sort subarray */
    merge_sort(lArr, 0, lLen);
    merge_sort(rArr, 0, rLen);
    int i = 0, j = 0, k = left;
    /* merge subarray */
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

/* merge sort entrance */
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
    res = read(fd, array, 4 * len);
    close(fd);
    sort(array, len);
    fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    res = write(fd, array, 4 * len);
    close(fd);
    return 0;
}
